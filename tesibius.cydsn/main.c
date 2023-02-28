#include <stdio.h>
#include "project.h"
#include "controls.h"
#include "debug.h"
#include "events.h"
#include "hardware_if.h"
#include "jiffy.h"
#include "keys.h"
#include "scheduler.h"

/*
static const char* const knob_names[] =
{
    [CONTROL_GAIN]     = "gain",
    [CONTROL_BITE]     = "bite",
    [CONTROL_BASS]     = "bass",
    [CONTROL_MID]      = "mid",
    [CONTROL_TREBLE]   = "treble",
    [CONTROL_PRESENCE] = "presence",
    [CONTROL_LEVEL]    = "level",
    [CONTROL_REVERB]   = "reverb",
    [CONTROL_MASTER]   = "master",
};

int main(void)
{
    CyGlobalIntEnable; // Enable global interrupts.
    uart_debug_Start();
    buttons_init();
    controls_init();
    leds_set(0);
    knobs_start();

    union knob_values knobs      = { { 0 } };
    union knob_values prev_knobs = { { 0 } };
    int buttons      = 0;
    int prev_buttons = 0;
    int gain = GAIN_CHANNEL_1;
    int leds = LED_CHANNEL_1;

    printf("Hello world!\r\n");
    pin_gain_ctrl_Write(gain);
    leds_set(leds);
    for ( ; ; )
    {
        CyDelay(100);
        if (knobs_get(&knobs))
        {
            for (int j = CONTROL_GAIN; j <= CONTROL_MASTER; ++j)
            {
                if (knobs.values[j] != prev_knobs.values[j])
                {
                    printf("%s %03hu\r\n", knob_names[j], knobs.values[j]);
                    controls_set(j, knobs.values[j]);
                }
            }
        }
        prev_knobs = knobs;
        buttons = buttons_scan();
        if (buttons != prev_buttons)
        {
            printf("buttons 0x%04X\r\n", buttons);
            int changed = buttons ^ prev_buttons;
            // On gain button up
            if ((changed & BUTTON_GAIN) && (prev_buttons & BUTTON_GAIN))
            {
                gain ^= GAIN_BIT;
                pin_gain_ctrl_Write(gain);
                leds ^= LED_CHANNEL_1|LED_CHANNEL_2;
                leds_set(leds);
                }
            prev_buttons = buttons;
        }
    }
}

*/

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define BUILD_NUM     1

static void
SysInit_()
{
    InitDebug();
    HalInit(); // Do this first to start up as quietly as possible
    EventsInit();
    KeysInit();
    /*    AnimationInit();
        MidiInit();
    */
    InitControls();
    HalStart();
    JiffyInit();
    DBG_PRINTF("\r\nTesibius, Copyright (c) Nick Twyman, 2020-2022\r\n");
    DBG_PRINTF("System version %d.%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION,
               BUILD_NUM);
    DBG_PRINTF("Flash size %d bytes\r\n", CY_FLASH_SIZE);
}

int
main()
{
    SysInit_();
    CyGlobalIntEnable;
    RunScheduler();
    return 0;
}
/* [] END OF FILE */
