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
#include "events.h"
#include "hardware_if.h"
#include "keys.h"

static uint16_t keys;
static uint16_t lastRead;
static uint32_t jiffiesLastRead;

#define DEBOUNCE_JIFFIES 5
// If a key is held down for more than 500 jiffies
// We treat this as a KEY_HOLD event
#define KEY_HOLD_JIFFIES 1000 

static int16_t keyDownTimeout[NUM_BUTTONS];

// Maps bit shifts to KeyCodes
static KeyCode keyCodeMap_[NUM_BUTTONS] =
    {
    KEY_PRESET_8, KEY_PRESET_7, KEY_PRESET_6, KEY_PRESET_5,
    KEY_PRESET_4, KEY_PRESET_3, KEY_PRESET_2, KEY_PRESET_1,
    KEY_SHIFT, KEY_GAIN
    };

void KeysInit()
{
    keys = lastRead = 0u;
    jiffiesLastRead = 0u;
    for (int i=0; i<NUM_BUTTONS; i++)
    {
        keyDownTimeout[i] = (-1);
    }
}

uint32_t elapsedJiffies(uint32_t now, uint32_t last)
{
    int64_t n = now;
    int64_t l = last;
    int64_t r = n - l;
    if (r < 0) 
    {
        r += 0x100000000ll;
    }
    return (uint32_t)r;
}

static void sendKeyEvents_(EventType upOrDown, uint16_t keysMask, uint32_t jiffy)
{
    for (int i=0; keysMask != 0; keysMask >>= 1, i++)
    {
        if (keysMask & 0x0001u) // Key has changed
        {
            KeyCode key = keyCodeMap_[i];
            if (upOrDown == KEY_UP)
            {
                if (keyDownTimeout[key] < 0)
                {
                    continue; // We already sent a KEY_HOLD event
                }
                keyDownTimeout[key] = (-1);
            }
            else if (upOrDown == KEY_DOWN)
            {
                keyDownTimeout[key] = KEY_HOLD_JIFFIES;
            }
            Event e;
            e.event = upOrDown;
            e.jiffy = jiffy;
            e.keyCode = key;
            QueueEvent(&e);// send event Event(upOrDown, keyCodeMap_[i])
        }
    }
}
void KeysJiffy(uint32_t jiffyCount)
{
    // First check if any keys have been held down for a while.
    for (int i=0; i<NUM_BUTTONS; i++)
    {
        int16_t timeout = keyDownTimeout[i];
        if ( timeout >= 0) // Key is down
        {
            
            timeout -= 1;
            keyDownTimeout[i] = timeout;
            if (timeout < 0)
            {
                Event e;
                e.event = KEY_HOLD;
                e.jiffy = jiffyCount;
                e.keyCode = i;
                QueueEvent(&e);
            }
        }
    }

    // Now scan the hardware
    uint16_t current = HalScanButtons();
    if (current != lastRead) // Reading is different from last time.
    {   
        // Track it and wait for things to settle
        lastRead = current;
        jiffiesLastRead = jiffyCount;
    }
    else if (current != keys &&
             elapsedJiffies(jiffyCount, jiffiesLastRead) >= DEBOUNCE_JIFFIES)
    { // New reading has been stable for 5 jiffies (approx 10ms)
        uint16_t keysChanged = current ^ keys;
        uint16_t keysDown = keysChanged & current;
        uint16_t keysUp = keysChanged & ~current;
        sendKeyEvents_(KEY_DOWN, keysDown, jiffyCount);

        sendKeyEvents_(KEY_UP, keysUp, jiffyCount);
        keys = current;
    }
}

/* [] END OF FILE */
