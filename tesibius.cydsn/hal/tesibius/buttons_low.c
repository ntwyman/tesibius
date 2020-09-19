#include "project.h"
#include "hal/buttons_low.h"

#define BUTTON_PIN_CLK  0x01u
#define BUTTON_PIN_PL   0x02u

void 
buttons_init(void)
{
    /* Set PL high, clock low. */
    pin_btn_op_Write(BUTTON_PIN_PL);
}

unsigned int
buttons_scan(void)
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
