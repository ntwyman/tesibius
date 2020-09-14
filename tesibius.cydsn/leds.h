#pragma once

/**
LED bit mask values for shift register. Refer to schematic.
*/
enum leds
{
    LED_NONE        = 0x0000,
    LED_PRESET_0    = 0x0001,
    LED_PRESET_1    = 0x0002,
    LED_PRESET_2    = 0x0004,
    LED_PRESET_3    = 0x0008,
    LED_PRESET_4    = 0x0010,
    LED_PRESET_5    = 0x0020,
    LED_PRESET_6    = 0x0040,
    LED_PRESET_7    = 0x0080,
    LED_CHANNEL_1   = 0x0100,
    LED_CHANNEL_2   = 0x0200,
    LED_SHIFT       = 0x0400,  
};

/**
Set the LEDs to the defined state. If the bit in value is a 1, then 
that LED will be lit.
@param value Bit set of LEDs to be illuminated.
*/
void
leds_set(int value);