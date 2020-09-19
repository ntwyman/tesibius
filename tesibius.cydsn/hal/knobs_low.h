#pragma once
/** @file Knob scanning low level driver. 
Most of the state machine runs in interrupt context.
*/
#include <stdint.h>
#include <stdbool.h>

#define KNOBS_COUNT             9
#define KNOBS_ADC_HYSTERESIS    16

/** 
Contains normalized 8 bit knob values.
*/
union knob_values
{
    struct 
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
    };
    uint8_t values[KNOBS_COUNT];
};

/**
Start the knob scanning process.
*/
void
knobs_start(void);
/**
Stop knob scanning.
*/
void
knobs_stop(void);
/**
Get the latest knob values.
@return true if values have changed since previous call.
*/
bool
knobs_get(union knob_values * values);