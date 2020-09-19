/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <cytypes.h>

typedef struct Task_ Task;
typedef enum TaskState_ TaskState;
typedef void (* TaskExecuter)(Task *);

enum TaskState_
{
    IDLE,
    PENDING,
    RUNNING
};

struct Task_ 
{
    Task*  next;
    TaskState  state;
    TaskExecuter run_method;
};

void
InitTask(Task* task, TaskExecuter run_method);

void
ScheduleTask(Task* task);

void
RunScheduler();
#endif // ifndef SCHEDULER_H
/* [] END OF FILE */
