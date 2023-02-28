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
#include "keys.h"
#include "controls.h"
#include "debug.h"

struct JiffyTask_ {
    Task baseTask;
    uint32_t jiffyCount;
};
static struct JiffyTask_ jiffyTask_;

CY_ISR(JiffyIsr)
{
    ScheduleTask(&jiffyTask_.baseTask);
    timer_jiffies_ClearInterrupt(timer_jiffies_GetInterruptSource());
    isr_jiffies_ClearPending();
}

static void JiffyTaskRunner_(Task* task)
{
    struct JiffyTask_* self = (struct JiffyTask_*)task;
    self->jiffyCount++;
    ControlsJiffy(self->jiffyCount);
    KeysJiffy(self->jiffyCount);
    // AnimationJiffy(self->jiffy_count);
}

void JiffyInit()
{
    InitTask(&jiffyTask_.baseTask, JiffyTaskRunner_);
    jiffyTask_.jiffyCount = 0;
    isr_jiffies_StartEx(JiffyIsr);
    timer_jiffies_Start();
}
/* [] END OF FILE */
