#include <stdio.h>

#include "project.h"
#include "buttons_low.h"
#include "controls.h"
#include "knobs_low.h"
#include "leds.h"
#include "macros.h"

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
    CyGlobalIntEnable; /* Enable global interrupts. */
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

/* [] END OF FILE */
