#ifndef _SPI_REGS_H_
#define _SPI_REGS_H_

#include <stdint.h>

#define SPI0_BASE 0x30000000
#define SPI1_BASE 0x30010000

#define SPI_RXDATA_OFFSET           0x0000
#define SPI_TXDATA_OFFSET           0x0004
#define SPI_CFG_OFFSET              0x0008
#define SPI_CTRL_OFFSET             0x000C
#define SPI_PR_OFFSET               0x0010
#define SPI_STATUS_OFFSET           0x0014
#define SPI_RX_FIFO_LEVEL_OFFSET    0xFE00
#define SPI_RX_FIFO_THRESHOLD_OFFSET 0xFE04
#define SPI_RX_FIFO_FLUSH_OFFSET    0xFE08
#define SPI_TX_FIFO_LEVEL_OFFSET    0xFE10
#define SPI_TX_FIFO_THRESHOLD_OFFSET 0xFE14
#define SPI_TX_FIFO_FLUSH_OFFSET    0xFE18
#define SPI_IM_OFFSET               0xFF00
#define SPI_MIS_OFFSET              0xFF04
#define SPI_RIS_OFFSET              0xFF08
#define SPI_IC_OFFSET               0xFF0C
#define SPI_GCLK_OFFSET             0xFF10

#define SPI_REG(base, offset) (*((volatile uint32_t*)((base) + (offset))))

#define SPI_RXDATA(base)            SPI_REG(base, SPI_RXDATA_OFFSET)
#define SPI_TXDATA(base)            SPI_REG(base, SPI_TXDATA_OFFSET)
#define SPI_CFG(base)               SPI_REG(base, SPI_CFG_OFFSET)
#define SPI_CTRL(base)              SPI_REG(base, SPI_CTRL_OFFSET)
#define SPI_PR(base)                SPI_REG(base, SPI_PR_OFFSET)
#define SPI_STATUS(base)            SPI_REG(base, SPI_STATUS_OFFSET)
#define SPI_RX_FIFO_LEVEL(base)     SPI_REG(base, SPI_RX_FIFO_LEVEL_OFFSET)
#define SPI_RX_FIFO_THRESHOLD(base) SPI_REG(base, SPI_RX_FIFO_THRESHOLD_OFFSET)
#define SPI_RX_FIFO_FLUSH(base)     SPI_REG(base, SPI_RX_FIFO_FLUSH_OFFSET)
#define SPI_TX_FIFO_LEVEL(base)     SPI_REG(base, SPI_TX_FIFO_LEVEL_OFFSET)
#define SPI_TX_FIFO_THRESHOLD(base) SPI_REG(base, SPI_TX_FIFO_THRESHOLD_OFFSET)
#define SPI_TX_FIFO_FLUSH(base)     SPI_REG(base, SPI_TX_FIFO_FLUSH_OFFSET)
#define SPI_IM(base)                SPI_REG(base, SPI_IM_OFFSET)
#define SPI_MIS(base)               SPI_REG(base, SPI_MIS_OFFSET)
#define SPI_RIS(base)               SPI_REG(base, SPI_RIS_OFFSET)
#define SPI_IC(base)                SPI_REG(base, SPI_IC_OFFSET)
#define SPI_GCLK(base)              SPI_REG(base, SPI_GCLK_OFFSET)

#define CFG_CPOL_BIT    0
#define CFG_CPHA_BIT    1

#define CTRL_SS_BIT     0
#define CTRL_EN_BIT     1
#define CTRL_RXEN_BIT   2

#define STATUS_TIP_BIT      0
#define STATUS_TXEMPTY_BIT  1
#define STATUS_TXFULL_BIT   2
#define STATUS_RXEMPTY_BIT  3
#define STATUS_RXFULL_BIT   4

static inline void spi_enable_clock(uint32_t base) {
    SPI_GCLK(base) = 0x1;
}

static inline void spi_set_prescaler(uint32_t base, uint8_t prescaler) {
    SPI_PR(base) = prescaler;
}

static inline void spi_set_config(uint32_t base, uint8_t cpol, uint8_t cpha) {
    SPI_CFG(base) = ((cpha & 0x1) << CFG_CPHA_BIT) | ((cpol & 0x1) << CFG_CPOL_BIT);
}

static inline void spi_enable(uint32_t base, uint8_t rx_en) {
    uint32_t ctrl = (1 << CTRL_EN_BIT);
    if (rx_en) {
        ctrl |= (1 << CTRL_RXEN_BIT);
    }
    SPI_CTRL(base) = ctrl;
}

static inline void spi_disable(uint32_t base) {
    SPI_CTRL(base) = 0x0;
}

static inline void spi_select_slave(uint32_t base) {
    uint32_t ctrl = SPI_CTRL(base);
    ctrl |= (1 << CTRL_SS_BIT);
    SPI_CTRL(base) = ctrl;
}

static inline void spi_deselect_slave(uint32_t base) {
    uint32_t ctrl = SPI_CTRL(base);
    ctrl &= ~(1 << CTRL_SS_BIT);
    SPI_CTRL(base) = ctrl;
}

static inline void spi_write_byte(uint32_t base, uint8_t data) {
    SPI_TXDATA(base) = data;
}

static inline uint8_t spi_read_byte(uint32_t base) {
    return SPI_RXDATA(base) & 0xFF;
}

static inline uint32_t spi_get_status(uint32_t base) {
    return SPI_STATUS(base);
}

static inline uint8_t spi_is_tx_empty(uint32_t base) {
    return (SPI_STATUS(base) >> STATUS_TXEMPTY_BIT) & 0x1;
}

static inline uint8_t spi_is_rx_empty(uint32_t base) {
    return (SPI_STATUS(base) >> STATUS_RXEMPTY_BIT) & 0x1;
}

static inline uint8_t spi_is_busy(uint32_t base) {
    return (SPI_STATUS(base) >> STATUS_TIP_BIT) & 0x1;
}

static inline void spi_flush_rx_fifo(uint32_t base) {
    SPI_RX_FIFO_FLUSH(base) = 0x1;
}

static inline void spi_flush_tx_fifo(uint32_t base) {
    SPI_TX_FIFO_FLUSH(base) = 0x1;
}

static inline void spi_init(uint32_t base, uint8_t cpol, uint8_t cpha, uint8_t prescaler) {
    spi_enable_clock(base);
    spi_set_prescaler(base, prescaler);
    spi_set_config(base, cpol, cpha);
    spi_flush_rx_fifo(base);
    spi_flush_tx_fifo(base);
    spi_enable(base, 1);
}

#endif
