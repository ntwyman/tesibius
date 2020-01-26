#include "buttons_low.h"
#include "project.h"

#define BUTTON_PIN_CLK 0x01
#define BUTTON_PIN_PL  0x02

void
vButtonInit(void)
{
    /* Set PL high, clock low. */
    pin_btn_op_Write(BUTTON_PIN_PL);
}

uint16_t
usButtonScan(void)
{
    int result = 0;
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
