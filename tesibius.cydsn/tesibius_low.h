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
#pragma once
/** Control identities. */
typedef enum
{
    TC_GAIN,
    TC_BITE,
    TC_BASS,
    TC_MID,
    TC_TREBLE,
    TC_PRESENCE,
    TC_LEVEL,
    TC_REVERB,
    TC_MASTER,
    TC_LEDS,
}  Control;

#define T_GAIN_CHANNEL_1 1
#define T_GAIN_CHANNEL_2 0
#define T_GAIN_BIT       1
#define NUM_PRESETS    8    

void
TesibiusSetControl(int id, int value);
/* [] END OF FILE */

