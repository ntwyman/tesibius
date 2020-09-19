/* ========================================
 *
 * Copyright Copyright Jonny Reckless & Nick Twyman, 2015-2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "scheduler.h"
#include "jiffy.h"
// #include "controls.h"
#include "keys.h"
#include "animation.h"
#include "debug.h"

struct JiffyTask_
{
    Task base_task;
    uint32 jiffy_count;
};
static struct JiffyTask_ jiffy_task_;

CY_ISR(JiffyIsr)
{
    ScheduleTask(&jiffy_task_.base_task);
    timer_jiffies_ClearInterrupt(timer_jiffies_GetInterruptSource());
    isr_jiffies_ClearPending();
}

static void
JiffyTaskRunner_(Task* task)
{
    struct JiffyTask_* self = (struct JiffyTask_*)task;
    self->jiffy_count++;
    KeysJiffy(self->jiffy_count);
    AnimationJiffy(self->jiffy_count);
}

void
JiffyInit()
{
    // queue_read_ptr = queue_write_ptr = &event_queue[0];

    InitTask(&jiffy_task_.base_task, JiffyTaskRunner_);
    jiffy_task_.jiffy_count = 0; 
    isr_jiffies_StartEx(JiffyIsr);
    timer_jiffies_Start();
}
/* [] END OF FILE */
