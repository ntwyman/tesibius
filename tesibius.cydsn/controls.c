#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"
#include "hardware_if.h"
#include "controls.h"
#include "leds.h"

typedef struct 
{
    uint8 gainChannel; 
    KnobValues knobs;
} AmpState;

static AmpState _state;

static char* knobNames[]=
{
    "GAIN", "BITE", "BASS", "MID", "TREBLE",
    "PRESENCE", "LEVEL", "REVERB", "MASTER"
};

void InitControls(void)
{
    memset(&_state, 0, sizeof(_state));
    InitLEDs(LED_CHANNEL_1);
    HalSetGain(_state.gainChannel);
}

void SetControl(Control id, uint8_t value)
{
    assert( (id < NUM_CTLS) && (id >= 0));
    HalSetControl(id, value);
    _state.knobs.values[id] = value; // Track current settings.
 
}

void ControlsJiffy(uint32_t jiffyCount)
{
    KnobValues knobs;
    bool haveChanged = HalGetKnobs(&knobs);
    if (haveChanged)
    {
        
        for (int i = CTL_GAIN; i< NUM_CTLS; i++)
        {
            if (_state.knobs.values[i] != knobs.values[i])
            {
                DBG_PRINTF("Control has changed - (ctl - %s, value - %u, jiffy - %u\r\n",
                    knobNames[i], knobs.values[i], jiffyCount);
                SetControl(i, knobs.values[i]);
            }
        }
    }
}

uint8 GetGainChannel()
{
    return _state.gainChannel;
}

void ToggleGainChannel()
{
    _state.gainChannel ^= 0x01;
    HalSetGain(_state.gainChannel);
    int leds = (_state.gainChannel == 0 ? LED_CHANNEL_1 : LED_CHANNEL_2);
    SetLEDs(leds, LED_CHANNEL_1 | LED_CHANNEL_2);
}