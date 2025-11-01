#include <firmware_apis.h>
#include "CF_SPI.h"

#define SPI1_BASE 0x30010000

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(false);

    GPIOs_configure(9, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(10, GPIO_MODE_USER_STD_INPUT_PULLUP);
    GPIOs_configure(11, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_configure(12, GPIO_MODE_USER_STD_OUTPUT);
    GPIOs_loadConfigs();

    User_enableIF();
    send_pulse(1);

    CF_SPI_setGclkEnable(SPI1_BASE, 1);
    CF_SPI_setPrescaler(SPI1_BASE, 2);
    CF_SPI_writepolarity(SPI1_BASE, 0);
    CF_SPI_writePhase(SPI1_BASE, 0);
    CF_SPI_enable(SPI1_BASE);
    CF_SPI_enableRx(SPI1_BASE);
    send_pulse(1);

    CF_SPI_assertCs(SPI1_BASE);
    CF_SPI_writeData(SPI1_BASE, 0x12);
    CF_SPI_waitTxFifoEmpty(SPI1_BASE);
    CF_SPI_waitNotBusy(SPI1_BASE);
    send_pulse(1);

    CF_SPI_writeData(SPI1_BASE, 0x34);
    CF_SPI_waitTxFifoEmpty(SPI1_BASE);
    CF_SPI_waitNotBusy(SPI1_BASE);
    send_pulse(1);

    CF_SPI_deassertCs(SPI1_BASE);
    CF_SPI_disable(SPI1_BASE);
    send_pulse(1);

    return;
}

static void send_pulse(int count) {
    for (int i = 0; i < count; i++) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
}
