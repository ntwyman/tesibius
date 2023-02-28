#pragma once
/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015-2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * ========================================
*/
#include <stdbool.h>
#include <stdint.h>
#define NUM_PRESETS 8    

typedef enum
{
    CTL_GAIN = 0,
    CTL_BITE,
    CTL_BASS,
    CTL_MID,
    CTL_TREBLE,
    CTL_PRESENCE,
    CTL_LEVEL,
    CTL_REVERB,
    CTL_MASTER,
    NUM_CTLS
}  Control;

typedef enum
{

    LED_PRESET_1    = 0x0001,
    LED_PRESET_2    = 0x0002,
    LED_PRESET_3    = 0x0004,
    LED_PRESET_4    = 0x0008,
    LED_PRESET_5    = 0x0010,
    LED_PRESET_6    = 0x0020,
    LED_PRESET_7    = 0x0040,
    LED_PRESET_8    = 0x0080,
    LED_CHANNEL_1   = 0x0100,
    LED_CHANNEL_2   = 0x0200,
    LED_SHIFT       = 0x0400,
    ALL_LEDS        = (LED_PRESET_1  | LED_PRESET_2 | LED_PRESET_3 |
                       LED_PRESET_4  | LED_PRESET_5 | LED_PRESET_6 |
                       LED_PRESET_7  | LED_PRESET_8 | LED_SHIFT    |
                       LED_CHANNEL_1 | LED_CHANNEL_2)
} LEDs;

typedef enum
{
    BUTTON_NONE     = 0x000,
    BUTTON_PRESET_1 = 0x080,
    BUTTON_PRESET_2 = 0x040,
    BUTTON_PRESET_3 = 0x020,
    BUTTON_PRESET_4 = 0x010,
    BUTTON_PRESET_5 = 0x008,
    BUTTON_PRESET_6 = 0x004,
    BUTTON_PRESET_7 = 0x002,
    BUTTON_PRESET_8 = 0x001,
    BUTTON_GAIN     = 0x200,
    BUTTON_SHIFT    = 0x100,
    BUTTON_MASK     = BUTTON_PRESET_1 | BUTTON_PRESET_2 | BUTTON_PRESET_3 |
                      BUTTON_PRESET_4 | BUTTON_PRESET_5 | BUTTON_PRESET_6 | 
                      BUTTON_PRESET_7 | BUTTON_PRESET_8 | BUTTON_GAIN | BUTTON_SHIFT,
} Buttons;
#define NUM_BUTTONS 10

typedef struct 
{
    uint8_t gain;
    uint8_t bite;
    uint8_t bass;
    uint8_t mid;
    uint8_t treble;
    uint8_t presence;
    uint8_t level;
    uint8_t reverb;
    uint8_t master;

} Knobs;

typedef union
{
    uint8_t values[NUM_CTLS];
    Knobs knob;
} KnobValues;

void HalInit(); // Brings up the amp in a muted safe state
void HalStart(); // Starts polling, interrupts etc.
void HalSetControl(Control id, int value);
void HalSetLEDs(int value);
void HalSetGain(int gainChannel);
bool HalGetKnobs(KnobValues *values);
uint16_t HalScanButtons();
void HalStop(); // Stops everything - probably only used when updating software.
/* [] END OF FILE */