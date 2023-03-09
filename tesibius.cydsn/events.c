/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "controls.h"
#include "debug.h"
#include "keys.h"
#include "events.h"
#include "scheduler.h"

#define EVENT_QUEUE_DEPTH 8

struct EventTask_ {
    Task baseTask;
    bool isFull;
    uint16_t writeIndex;
    uint16_t readIndex;
    Event eventQueue[EVENT_QUEUE_DEPTH];
};
static struct EventTask_ eventTask_;

static void processEvent_(Event *e)
{
    switch (e->event)
    {
        case KEY_DOWN:
            DBG_PRINTF("Key down (key - %d, jiffy: %u)\r\n", e->keyCode, e->jiffy);
            break;
        case KEY_UP:
            DBG_PRINTF("Key up (key - %d, jiffy: %u)\r\n", e->keyCode, e->jiffy);
            if (e->keyCode == KEY_GAIN)
            {
                ToggleGainChannel();
            }
            else if (e->keyCode >= KEY_PRESET_1 && e->keyCode <= KEY_PRESET_8)
            {
                int preset = e->keyCode - KEY_PRESET_1;
                DBG_PRINTF("Loading Preset %d", preset);
                LoadPreset(preset);
            }
            
            break;
        case KEY_HOLD:
            DBG_PRINTF("Key hold (key - %d, jiffy: %u)\r\n", e->keyCode, e->jiffy);
             if (e->keyCode >= KEY_PRESET_1 && e->keyCode <= KEY_PRESET_8)
            {
                int preset = e->keyCode - KEY_PRESET_1;
                DBG_PRINTF("Saving Preset %d", preset);
                SavePreset(preset);
            }
            break;
        default:
            DBG_PRINTF("Unknown event - (event: %d, jiffy: %u)\r\n", e->event, e->jiffy);
    }
}
 
static void EventTaskRunner_(Task* task)
{
    struct EventTask_* self = (struct EventTask_*)task;
    do
    {
        processEvent_(&self->eventQueue[self->readIndex++]);
        if (self->readIndex >= EVENT_QUEUE_DEPTH)
        {
            self->readIndex=0;
        }
    } while (self->readIndex != self->writeIndex);
    self->isFull = false;
}

void EventsInit()
{
    eventTask_.writeIndex = 0;
    eventTask_.readIndex = 0;
    eventTask_.isFull = false;
    InitTask(&eventTask_.baseTask, EventTaskRunner_);
};


void QueueEvent(Event *e)
{
    assert(e != NULL);
    DBG_PRINTF("Adding event type %d\r\n", e->event);
    if (eventTask_.isFull)
    {
        DBG_PRINTF("XXXXXXXXX EVENT QUEUE IS FULL XXXXXXX\r\n");
    }
    memcpy(&eventTask_.eventQueue[eventTask_.writeIndex], e, sizeof(Event));
    eventTask_.writeIndex++;
    if (eventTask_.writeIndex >= EVENT_QUEUE_DEPTH) 
    {
        eventTask_.writeIndex = 0;
    }
    eventTask_.isFull = (eventTask_.writeIndex == eventTask_.readIndex);
    ScheduleTask(&eventTask_.baseTask);
}

 
/* [] END OF FILE */
