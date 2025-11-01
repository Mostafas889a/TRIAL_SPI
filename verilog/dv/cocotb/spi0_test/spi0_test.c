#include <firmware_apis.h>
#include "CF_SPI.h"

#define SPI0_BASE 0x30000000

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(false);

    GPIOs_configure(5, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(6, GPIO_MODE_USER_STD_INPUT_PULLUP);
    GPIOs_configure(7, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(8, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_loadConfigs();

    User_enableIF();
    send_pulse(1);

    CF_SPI_setGclkEnable(SPI0_BASE, 1);
    CF_SPI_setPrescaler(SPI0_BASE, 2);
    CF_SPI_writepolarity(SPI0_BASE, 0);
    CF_SPI_writePhase(SPI0_BASE, 0);
    CF_SPI_enable(SPI0_BASE);
    CF_SPI_enableRx(SPI0_BASE);
    send_pulse(1);

    CF_SPI_assertCs(SPI0_BASE);
    CF_SPI_writeData(SPI0_BASE, 0xA5);
    CF_SPI_waitTxFifoEmpty(SPI0_BASE);
    CF_SPI_waitNotBusy(SPI0_BASE);
    send_pulse(1);

    CF_SPI_writeData(SPI0_BASE, 0x5A);
    CF_SPI_waitTxFifoEmpty(SPI0_BASE);
    CF_SPI_waitNotBusy(SPI0_BASE);
    send_pulse(1);

    CF_SPI_deassertCs(SPI0_BASE);
    CF_SPI_disable(SPI0_BASE);
    send_pulse(1);

    return;
}

static void send_pulse(int count) {
    for (int i = 0; i < count; i++) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
}
