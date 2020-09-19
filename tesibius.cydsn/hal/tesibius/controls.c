#include <stdio.h>
#include "project.h"
#include "hal/controls.h"

#define DIG_POT_CMD 0x11

void
controls_init(void)
{
    pin_spi_addr_Write(0);
    spi_master_Start();
}

void
controls_set(int id, int value)
{
    
    while (!(spi_master_ReadTxStatus() & spi_master_STS_SPI_IDLE))
        ;
    pin_spi_addr_Write(id);
    CyDelayUs(1);
    spi_master_WriteByte(DIG_POT_CMD); // Addr 00, WRITE
    spi_master_WriteByte(value);
    printf("Set control %d to %d\r\n", id, value);
}
