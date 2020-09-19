#include <assert.h>
#include <stdlib.h>

#include "project.h"
#include "hal/knobs_low.h"
#include "hal/controls.h"

struct knobs_ctx
{
    int     channel_num;
    int     adc_values[KNOBS_COUNT];
    bool    have_knobs_changed;
};

static struct knobs_ctx ctx;

CY_ISR(on_adc_conversion_complete)
{
    const int channel = ctx.channel_num;
    if (++ctx.channel_num == KNOBS_COUNT)
    {
        ctx.channel_num = 0;
    }
    /* Switch the analog mux ASAP to give it time to settle. */
    mux_analog_Select(ctx.channel_num);
    int value = adc_sar_GetResult16(0);
    if (value < 0)
    {
        value = 0;
    }
    if (abs(value - ctx.adc_values[channel]) > KNOBS_ADC_HYSTERESIS)
    {
        ctx.have_knobs_changed = true;
        ctx.adc_values[channel] = value;
    }
    isr_adc_ClearPending();
    adc_sar_StartConvert();    
}

void
knobs_start(void)
{
    memset(&ctx, 0, sizeof(ctx));
    mux_analog_Start();
    mux_analog_Select(0);
    adc_sar_Start();
    isr_adc_StartEx(on_adc_conversion_complete);
    adc_sar_StartConvert();
}

void
knobs_stop(void)
{
    const uint8_t x = CyEnterCriticalSection();
    isr_adc_Stop();
    adc_sar_Stop();
    isr_adc_ClearPending();
    CyExitCriticalSection(x);
}

bool
knobs_get(union knob_values * values)
{
    assert(values != NULL);
    for (int i = CONTROL_GAIN; i <= CONTROL_MASTER; ++i)
    {
        values->values[i] = (uint8_t)(ctx.adc_values[i] >> 5);
    }
    const bool has_changed = ctx.have_knobs_changed;
    ctx.have_knobs_changed = false;
    return has_changed;
}