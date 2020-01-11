#include "leds.h"
#include "controls.h"
#include "project.h"

void
leds_set(int value)
{
    value = ~value; // LEDs are active low
    while (!(spi_master_ReadTxStatus() & spi_master_STS_SPI_IDLE))
        ;
    pin_spi_addr_Write(CONTROL_LEDS);
    CyDelayUs(1);
    spi_master_WriteByte(value >> 8);
    spi_master_WriteByte(value);
}