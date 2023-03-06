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

typedef enum {
    AUDIO,
    LINEAR,
    CAUDIO
} Taper;

typedef struct {
    Taper taper : 2;
} PackedTaper;

static const PackedTaper CYCODE controlTaper[NUM_CTLS] = 
{
    {AUDIO},   // CTL_GAIN
    {CAUDIO},  // CTL_BITE
    {AUDIO},   // CTL_BASS
    {LINEAR},  // CTL_MID
    {LINEAR},  // CTL_TREBLE
    {LINEAR},  // CTL_PRESENCE
    {AUDIO},  // CTL_LEVEL
    {LINEAR},  // CTL_REVERB
    {AUDIO}    // CTL_MASTER
};

static const uint8 CYCODE audioTaper[256] = 
{
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  9,  9,  9, 10, 10, 10,
     10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12,
     12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15,
     15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19,
     19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24,
     24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29,
     30, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36,
     37, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45,
     46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56,
     57, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 69,
     70, 71, 72, 73, 74, 75, 76, 77, 78, 80, 81, 82, 83, 84, 85, 86,
     87, 89, 90, 91, 92, 94, 95, 96, 97, 99,100,102,103,104,106,107,
    109,110,112,113,115,116,118,119,121,123,124,126,128,130,131,133,
    135,137,139,141,142,144,146,148,150,152,154,157,159,161,163,165,
    168,170,172,175,177,179,182,184,187,189,192,194,197,200,203,205,
    208,211,214,217,220,223,226,229,232,235,238,242,245,248,252,255,
};

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
    assert(value >=0 && value <256);
    while (!(spi_master_ReadTxStatus() & spi_master_STS_SPI_IDLE))
        ;
    pin_spi_addr_Write(id);
    Taper taper = controlTaper[id].taper;
    if (taper == CAUDIO) 
    {
        value = 255 - audioTaper[255-value];
    } 
    else if (taper == AUDIO)
    {
        value = audioTaper[value];
    }
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
