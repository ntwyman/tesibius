#include <stdlib.h>
#include <CyLib.h>
#include <CyPm.h>
#include "scheduler.h"

// static Task* _active_task = NULL; Not sure if we'll need this.

static Task* pending_tasks_ = NULL;
static Task* pending_tasks_tail_ = NULL;

void
InitTask(Task* task, TaskExecuter runner)
{
    task->next = NULL;
    task->state = IDLE;
    task->run_method = runner;
}

void
ScheduleTask(Task *task)
{
    uint8 enterStatus = CyEnterCriticalSection();
    if (task->state != PENDING) // Only put a task on the queue once
    {
        if (pending_tasks_tail_ != NULL) // We have tasks pending
        {
            pending_tasks_tail_->next = task;
        }
        else
        {
            pending_tasks_ = task;
        }
        pending_tasks_tail_ = task;
        task->state = PENDING;
        task->next = NULL; // Just in case
    }
    CyExitCriticalSection(enterStatus);
}

void
RunScheduler()
{
    for (;;)
    {
        while (pending_tasks_ == NULL)
        {
            CySysPmSleep();
        }

        int8 enterStatus = CyEnterCriticalSection();
        Task* runnable = pending_tasks_;
        pending_tasks_ = pending_tasks_->next;
        if (runnable == pending_tasks_tail_) // reached the end of the line
        {
            pending_tasks_tail_ = NULL;
        }
        runnable->state = RUNNING;
        CyExitCriticalSection(enterStatus);
                
        (*(runnable->run_method))(runnable);
        
        enterStatus = CyEnterCriticalSection();
        if (runnable->state == RUNNING) // May have been queued to pending again while running
        {
            runnable->state = IDLE;
        }
        CyExitCriticalSection(enterStatus);
    }   
}
/* [] END OF FILE */
