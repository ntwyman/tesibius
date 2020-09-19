/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015-2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "debug.h"
#include "animation.h"
#include "controls.h"

typedef struct _animationStep
{
	uint16 led_mask;  // LEDs affected
	uint16 led_state; // LED state
	uint16 duration;  // in jiffies (approx 20 mS)
} AnimationStep;

#define NO_ANIMATION ((AnimationStep *)0)
static const AnimationStep *pNextStep = NO_ANIMATION;
static uint16 save_state;
static uint32 last_jiffy;
static uint32 end_time;

static const AnimationStep CYCODE save_animation[] =
{
	{ ALL_LEDS, ALL_LEDS, 300},
	{ ALL_LEDS, 0, 250},
	{ ALL_LEDS, ALL_LEDS, 300},
	{ 0, 0 , 0}
};

static void runStep()
{
	DBG_PRINTF("Animation step - mask(%04x), state(%04x), duration(%dmS)\r\n", pNextStep->led_mask, pNextStep->led_state, pNextStep->duration);
	if (pNextStep->duration == 0) // All done
	{
		pNextStep = NO_ANIMATION; // So know we're between animations
		SetLEDState(save_state);
        return;
	}

	end_time = last_jiffy + pNextStep->duration;
    DBG_PRINTF("Endtime %08x\r\n", end_time);
	uint16 state = GetLEDState();
	state &= ~pNextStep->led_mask;
	state |= pNextStep->led_state;
	SetLEDState(state);
    pNextStep++;
}

void
AnimationInit()
{
	// TODO
}

void
AnimationJiffy(uint32 jiffy_count)
{
	last_jiffy = jiffy_count;
	if ((pNextStep == NO_ANIMATION) || (jiffy_count != end_time))
	{
		return;
	}
	runStep();
}

void
RunAnimation(Animation animation)
{
	if (animation == SetChannel)
	{
		if (pNextStep == NO_ANIMATION)  // Not in the middle of an animation
		{
			save_state = GetLEDState();
		}
		pNextStep = save_animation;
		runStep();
	}
}
/* [] END OF FILE */
