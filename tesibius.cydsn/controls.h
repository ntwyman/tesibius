#pragma once

/** Control identities. */
enum controls
{
    CONTROL_GAIN,
    CONTROL_BITE,
    CONTROL_BASS,
    CONTROL_MID,
    CONTROL_TREBLE,
    CONTROL_PRESENCE,
    CONTROL_LEVEL,
    CONTROL_REVERB,
    CONTROL_MASTER,
    CONTROL_LEDS,
};  

#define GAIN_CHANNEL_1  1
#define GAIN_CHANNEL_2  0
#define GAIN_BIT 1

/**
Initialize controls.
*/
void
controls_init(void);

/**
Set a control value
@param id which control
@param value The value to send
*/
void
controls_set(int id, int value);