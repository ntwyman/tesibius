/*
 * input.h
 * The Input handling for the Tesibius digitally controlled amplifier.
 * This sets up a periodic task to scan the keyboard every 50ms and
 * primes and reads the ADC values for the knobs in a loop
 * copyright (c) Nick Twyman, 2020.
 */

#include "hal/buttons_low.h"

// Initialize & start the keyboard scanning timer
void
vInputInit()
