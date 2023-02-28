/* ========================================
 *
 * Copyright Nick Twyman, 2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * ========================================
*/
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "project.h"
#include "hardware_if.h"

// Control Knobs
#define DIG_POT_CMD 0x11
#define KNOB_ADC_HYSTERESIS 8

static struct
{
    int     adcChannel;
    int     adcValues[NUM_CTLS];
    bool    haveChanged;
} _knobsState;

// LEDs
#define LED_SPI_ADDR (CTL_MASTER + 1)

// Buttons
#define BUTTON_PIN_CLK  0x01u
#define BUTTON_PIN_PL   0x02u

// Bare minimum to bring up the amp in quiescent state
void HalInit()
{
    pin_spi_addr_Write(0);
    spi_master_Start();
    HalSetControl(CTL_MASTER, 0); // early as possible so we come up quietly
    
    memset(&_knobsState, 0, sizeof(_knobsState));

    /* Button drivers - Set PL high, clock low. */
    pin_btn_op_Write(BUTTON_PIN_PL);

}

CY_ISR(_onAdcConversionComplete)
{
    const int channel = _knobsState.adcChannel;
    if (++_knobsState.adcChannel == NUM_CTLS)
    {
        _knobsState.adcChannel = 0;
    }
    /* Switch the analog mux ASAP to give it time to settle. */
    mux_analog_Select(_knobsState.adcChannel);
    int value = adc_sar_GetResult16(0);
    if (value < 0)
    {
        value = 0;
    }
    if (abs(value - _knobsState.adcValues[channel]) > KNOB_ADC_HYSTERESIS)
    {
        _knobsState.haveChanged = true;
        _knobsState.adcValues[channel] = value;
    }
    isr_adc_ClearPending();
    adc_sar_StartConvert();    
}

void HalStart()
{
    mux_analog_Start();
    mux_analog_Select(0);
    adc_sar_Start();
    isr_adc_StartEx(_onAdcConversionComplete);
    adc_sar_StartConvert();
}

void HalSetControl(Control id, int value)
{
    while (!(spi_master_ReadTxStatus() & spi_master_STS_SPI_IDLE))
        ;
    pin_spi_addr_Write(id);
    CyDelayUs(1);
    spi_master_WriteByte(DIG_POT_CMD); // Addr 00, WRITE
    spi_master_WriteByte(value);
}

void HalSetLEDs(int value)
{    
    value = ~value; // Bits are active low
    while (!(spi_master_ReadTxStatus() & spi_master_STS_SPI_IDLE))
        ;
    pin_spi_addr_Write(LED_SPI_ADDR);
    CyDelayUs(1);  
    spi_master_WriteByte(value >> 8);
    spi_master_WriteByte(value);
}

void HalSetGain(int value)
{
    // Looks like bit high is low gain
    pin_gain_ctrl_Write(value ^ 0x01);
 
}

bool HalGetKnobs(KnobValues *knobs)
{
    assert(knobs != NULL);
    for (int i=0; i<NUM_CTLS; i++)
    {
        knobs->values[i] = _knobsState.adcValues[i] >> 5;
    }
    bool haveChanged = _knobsState.haveChanged;
    _knobsState.haveChanged = false;
    return haveChanged;
}

uint16_t HalScanButtons()
{
    unsigned int result = 0;
    /* Pulse the parallel load pin low for 1us to latch the register */
    pin_btn_op_Write(0);
    CyDelayUs(1);
    pin_btn_op_Write(BUTTON_PIN_PL);
    CyDelayUs(1);
    /* Bit bash the serial shift. Data changes on the rising edge of clock. */
    for (int i = 15; i >= 0; --i)
    {
        result = (result << 1) | pin_btn_ip_Read();
        pin_btn_op_Write(BUTTON_PIN_PL | BUTTON_PIN_CLK);
        CyDelayUs(1);
        pin_btn_op_Write(BUTTON_PIN_PL);
        CyDelayUs(1);    
    }
    result = ~result & BUTTON_MASK; /* Button hardware is active low */
    return result;
}

// Stops everything - probably only used when updating software.
void HalStop()
{
    const uint8_t x = CyEnterCriticalSection();
    isr_adc_Stop();
    adc_sar_Stop();
    isr_adc_ClearPending();
    CyExitCriticalSection(x);
}; 
/* [] END OF FILE */
