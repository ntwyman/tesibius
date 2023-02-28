#pragma once
/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015 - 2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <stdint.h>
typedef enum
{
    KEY_DOWN = 0,
    KEY_UP,
    KEY_HOLD
} EventType;

typedef enum
{
    KEY_PRESET_1 = 0,
    KEY_PRESET_2,
    KEY_PRESET_3,
    KEY_PRESET_4,
    KEY_PRESET_5,
    KEY_PRESET_6,
    KEY_PRESET_7,
    KEY_PRESET_8,
    KEY_GAIN,
    KEY_SHIFT
} KeyCode;

typedef struct
{
    EventType event;
    uint32_t jiffy;
    union
    {
        KeyCode keyCode;
    };
} Event;

void EventsInit();
void QueueEvent(Event *e);
