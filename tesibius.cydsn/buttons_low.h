#pragma once
/** @file Low level button scanning driver. 
Debounce, long press etc should be handled higher up.
Button scanning uses 74HC165 shift registers which are not 
SPI(0,0) compatible, so the scanning code bit-bashes 
the serial interface.
*/

/**
Individual button bit masks. Refer to schematic.
*/
enum button_masks
{
    BUTTON_NONE     = 0x000,
    BUTTON_PRESET_0 = 0x080,
    BUTTON_PRESET_1 = 0x040,
    BUTTON_PRESET_2 = 0x020,
    BUTTON_PRESET_3 = 0x010,
    BUTTON_PRESET_4 = 0x008,
    BUTTON_PRESET_5 = 0x004,
    BUTTON_PRESET_6 = 0x002,
    BUTTON_PRESET_7 = 0x001,
    BUTTON_GAIN     = 0x200,
    BUTTON_SHIFT    = 0x100,
    BUTTON_MASK     = BUTTON_PRESET_0 | BUTTON_PRESET_1 | BUTTON_PRESET_2 |
                      BUTTON_PRESET_3 | BUTTON_PRESET_4 | BUTTON_PRESET_5 | 
                      BUTTON_PRESET_6 | BUTTON_PRESET_7 | BUTTON_GAIN | BUTTON_SHIFT,
};
/**
Perform one-time initialization of the buttons.
*/
void 
buttons_init(void);
/**
Perform a raw scan of the buttons (no debounce).
This will take approximately 34us to complete.
@return The current state of the buttons.
*/
int
buttons_scan(void);