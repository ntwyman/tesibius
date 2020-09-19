/* ========================================
 *
 * Copyright Copyright Jonny Reckless & Nick Twyman, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "scheduler.h"
#include "adc.h"
#include "controls.h"
#include "debug.h"

enum AdcInputs_ {
    ADC_GAIN = 0,
    ADC_BASS,
    ADC_MID,
    ADC_TREBLE,
    ADC_SEND_LEVEL,
    ADC_REVERB,
    ADC_MASTER,
    NUM_ADC_INPUTS
};
#define ADC_NOISE_THRESHOLD 32
#define ADC_INIT_VALUE 0x4000

struct AdcTask_
{
    Task base_task;
    int16 last_values[NUM_ADC_INPUTS];
};

static struct AdcTask_ adc_task_;

CY_ISR(AdcIsr)
{
    ScheduleTask(&adc_task_.base_task);
    adc_sar_IRQ_ClearPending();
    isr_adc_ClearPending();
}

static void
AdcTaskRunner_(Task* task)
{
    struct AdcTask_* self = (struct AdcTask_*)task;
    ChannelTarget new_values[NUM_ADC_INPUTS];
    unsigned int update_count=0;
    unsigned int update_index=0;
    // Work out which channels have changed/need updating
    uint32 channel;
    for (channel=ADC_GAIN; channel<NUM_ADC_INPUTS; channel++)
    {
        int16 new_value = adc_sar_GetResult16(channel);
        if (new_value < 0) // Clamp reading as we sometimes get tiny negative readings
        {
            new_value = 0;
        }
        
        int needs_update = 0;
        int16 old_value = self->last_values[channel];
        if (old_value == ADC_INIT_VALUE) // First time always update the control
        {
            needs_update = 1;
        } else {
            int diff = old_value - new_value;
            if (diff < 0) diff = -diff;
            if (diff > ADC_NOISE_THRESHOLD)
            {
               needs_update=1; 
            }
        }
        // Collect values that need updating
        if (needs_update != 0) {
            DBG_PRINTF("Updating value for channel %d to %04x\r\n", channel, new_value);
            new_values[update_count].channel_num = channel;
            new_values[update_count].value = new_value;
            update_count++;
            self->last_values[channel] = new_value;
        }
    }
    
    // Update any controls that have changed
    for (update_index=0; update_index < update_count; update_index++)
    {
        SetControl(new_values[update_index].channel_num, new_values[update_index].value >> 5);
    }
}

void
AdcInit()
{
    uint16 idx;
    for (idx=0; idx<(sizeof(adc_task_.last_values)/sizeof(uint16)); idx++)
    {
        adc_task_.last_values[idx] = ADC_INIT_VALUE;
    }
    InitTask(&adc_task_.base_task, AdcTaskRunner_);
    isr_adc_StartEx(AdcIsr);
    adc_sar_Start();
    adc_sar_StartConvert(); // In free-running mode
    adc_sar_IRQ_Enable();
}
/* [] END OF FILE */
