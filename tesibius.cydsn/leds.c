#include "project.h"
#include "leds.h"
#include "hardware_if.h"

static int _ledState = 0x0000;

int InitLEDs(int state) 
{
    return SetLEDs(state, ALL_LEDS);
}

int SetLEDs(int value, int mask)
{
    // Update the state
    _ledState &= ~mask;
    _ledState |= (value & mask);

    // Set the LEDs
    HalSetLEDs(_ledState);
    return _ledState;
}

int TurnOnLEDs(int mask)
{
    return SetLEDs(mask, mask);
}

int TurnOffLEDs(int mask)
{
    return SetLEDs(0, mask);
}

int GetLEDState()
{
    return _ledState;
}