#pragma once
#include <stdint.h>
#include "hardware_if.h"

/**
Initialize controls.
*/
void InitControls(void);

/**
Set a control value
@param id which control
@param value The value to send
*/
void SetControl(Control id, uint8_t value);

/**
Called by Jiffy to update controls
@param jiffyCount - # jiffies since last reset
*/
void ControlsJiffy(uint32_t jiffyCount);

void ToggleGainChannel();

void SavePreset(int preset);
void LoadPreset(int preset);