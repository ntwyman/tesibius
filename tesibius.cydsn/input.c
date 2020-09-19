#include "hal/include/buttons_low.h"
#include "macros.h"
#include <project.h>
#include "timers.h"

#define KEY_SCAN_MS      50
#define KEY_DEBOUNCE_MS  100
#define KEY_HOLD_TIME_MS 2000 // Button is HELD if pressed for 2 seconds

SysTick_Type xLastScanTick = 0;
SysTick_Type xLastChangeTick;
SysTick_Type xLastChangeTick;
uint16_t usKeysAtLastChange;
uint16_t usLastKeysDown;
uint16_t usKeyDownTimes[NUM_BUTTONS];
uitn16_t usKeyEventSent;

int32_t
lTickDeltaInMs(SysTick_Type xNow, SysTick_Type xThen)
{
    int64_t delta = (int64_t)xCurrentTick - (int64_t)xLastChangeTick;
    while (delta < 0)
    {
        delta += ((int64_t)1) << sizeof(SysTick_Type)
    }
    return portTICK_PERIOD_MS * (int32_t)delta;
}

bool
bDebounceLeys(uint16_t usKeysDown, SysTick_Type xCurrentTick)
{
    SysTick_Type xCurrentTick = xTaskGetTickCount();
    if (xLastTickCount == 0 || usKeysAtLastChange != usKeysDown)
    {
        usKeysAtLastChange = usKeysDown;
        xLastChangeTick = xCurrentTick;
        return false;
    }
    return lTickDeltaInMs(xCurrentTick, xLastChangeTick) > KEY_DEBOUNCE_MS;
}

void
vScanKeyboard()
{
    uint16_t usKeysDown = button_scan(); //
    SysTick_Type xCurrentTick = xTaskGetTickCount();

    if (!bDebounceKeys(usKeysDown, xCurrentTick))
    {
        return;
    }

    uint16_t usKeysChanged = usKeysDown ^ usLastStableKeys;
    uint16_t usKeysReleased = usKeysChanged & usLastKeysDown;
    uint16_t usNewKeysDown = usKeysChanged & usKeysDown;
    usLastKeysDown = usKeysDown;

    // Record the key_down time for new keys down
    int keyIndex;
    for (keyIndex = 0; usNewKeysDown != 0; keyIndex++, usNewKeysDown >>= 1)
    {
        if (usNewKeysDown & 0x0001)
        {
            usKeyDownTimes[keyIndex] = xCurrentTick;
        }
    }

    // We send a KEY_HOLD event every two seconds when the keys is held down
    for (keyIndex = 0; usKeysDown != 0; keyIndex++, usKeysDown >>= 1)
    {
        if ((usKeysDown & 0x0001) &&
            lTickDeltaInMs(xCurrentTick, usKeyDownTimes[keyIndex]) >=
                KEY_HOLD_TIME_MS)
        {
            uint16_t usKeyMask = (0x0001 << keyIndex);
            // Send KEY_HOLD
            ControlsKey(usKeyMask, KEY_HOLD);
            // Stop a KEY_PRESS event from occuring
            usKeyEventSent |= usKeyMask;
        }
    }

    // We send a KEY_PRESS when a key is pressed and released less than 2
    // seconds later.
    for (keyIndex = 0; usKeysReleased != 0; keyIndex++, usKeysReleased >>= 1)
    {
        if (usKeysReleased & 0x0001) // Key is newly released
        {                            // Key is newly released
            uint16_t usKeyMask = (0x0001 << keyIndex);
            if (usKeyEventSent & usKeyMask)
            {
                usKeyEventSent &= ~usKeyMask;
                continue;
            }
            ControlsKey(usKeyMask, KEY_PRESS);
        }
    }
}

void
vInputInit()
{
    TimerHandle_t xKeyboardTimer =
        xTimerCreate("Keyboard Scan", pdMS_TO_TICKS(KEY_SCAN_MS), pdTRUE,
                     (void*)0, vScanKeyboard);
    INIT_ASSERT(xKeyboardTimer != NULL, "Creating keyboard scan timer");

    BaseType_t xStatus = xTimerStart(xKeyboardTimer, NULL);
    INIT_ASSERT(xStatus == pdPASS, "Starting keyboard scan timer");
    vButtonsInit();
}