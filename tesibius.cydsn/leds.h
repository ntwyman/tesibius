#pragma once
/**
Initializes the LEDs with the passed value.
Is equivilent to SetLEDs(value, MASK_ALL_LEDs)
@param value Bit set of LEDs to be illuminated.
*/
int InitLEDs(int value);

/**
Set the masked LEDs to the defined state.
If the bit in value is a 1, then that LED will be lit.
@param value Bit set of LEDs to be illuminated.
@param mask Bit set of LEDs states to be updates
*/
int SetLEDs(int value, int mask);

/**
Get the current state of the LEDs
*/
int GetLEDState();

int TurnOnLEDs(int mask);

int TurnOffLEDs(int mask);
