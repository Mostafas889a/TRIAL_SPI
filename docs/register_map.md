# Register Map - TRIAL_SPI

## Overview
This document describes the register map for the TRIAL_SPI Caravel user project, which integrates 2 x SPI controllers into the Caravel SoC.

## Address Space
- **User Project Base Address**: 0x3000_0000
- **Address Space per Peripheral**: 64 KB (0x10000)

## Peripheral Base Addresses

| Peripheral | Base Address | Address Range | Description |
|-----------|-------------|---------------|-------------|
| SPI0 | 0x3000_0000 | 0x3000_0000 - 0x3000_FFFF | First SPI controller |
| SPI1 | 0x3001_0000 | 0x3001_0000 - 0x3001_FFFF | Second SPI controller |

## SPI Controller Register Map

Each SPI controller (CF_SPI_WB) has the following register map. Add the peripheral base address to the offset to get the absolute address.

### Data Registers

| Register | Offset | Reset Value | Access | Description |
|----------|--------|-------------|--------|-------------|
| RXDATA | 0x0000 | 0x00000000 | R | RX Data register; interface to the Receive FIFO |
| TXDATA | 0x0004 | 0x00000000 | W | TX Data register; interface to the Transmit FIFO |

### Configuration Registers

| Register | Offset | Reset Value | Access | Description |
|----------|--------|-------------|--------|-------------|
| CFG | 0x0008 | 0x00000000 | W | Configuration Register |
| CTRL | 0x000C | 0x00000000 | W | Control Register |
| PR | 0x0010 | 0x00000002 | W | SPI clock Prescaler (value >= 2); SPI Clock = System Clock / PR |
| STATUS | 0x0014 | 0x00000000 | R | Status register |

### FIFO Registers

| Register | Offset | Reset Value | Access | Description |
|----------|--------|-------------|--------|-------------|
| RX_FIFO_LEVEL | 0xFE00 | 0x00000000 | R | RX FIFO Level Register |
| RX_FIFO_THRESHOLD | 0xFE04 | 0x00000000 | W | RX FIFO Level Threshold Register |
| RX_FIFO_FLUSH | 0xFE08 | 0x00000000 | W | RX FIFO Flush Register |
| TX_FIFO_LEVEL | 0xFE10 | 0x00000000 | R | TX FIFO Level Register |
| TX_FIFO_THRESHOLD | 0xFE14 | 0x00000000 | W | TX FIFO Level Threshold Register |
| TX_FIFO_FLUSH | 0xFE18 | 0x00000000 | W | TX FIFO Flush Register |

### Interrupt Registers

| Register | Offset | Reset Value | Access | Description |
|----------|--------|-------------|--------|-------------|
| IM | 0xFF00 | 0x00000000 | W | Interrupt Mask Register; write 1/0 to enable/disable interrupts |
| MIS | 0xFF04 | 0x00000000 | R | Masked Interrupt Status |
| RIS | 0xFF08 | 0x00000000 | R | Raw Interrupt Status |
| IC | 0xFF0C | 0x00000000 | W | Interrupt Clear Register; write 1 to clear interrupt |
| GCLK | 0xFF10 | 0x00000000 | W | Gated clock enable; 1: enable clock, 0: disable clock |

## Register Field Details

### CFG Register (Offset: 0x0008)
Configuration Register for SPI protocol settings.

| Bit(s) | Field | Type | Reset | Description |
|--------|-------|------|-------|-------------|
| 0 | CPOL | RW | 0 | SPI Clock Polarity (0: idle low, 1: idle high) |
| 1 | CPHA | RW | 0 | SPI Clock Phase (0: sample on first edge, 1: sample on second edge) |
| 31:2 | Reserved | - | 0 | Reserved |

### CTRL Register (Offset: 0x000C)
Control Register for SPI operation.

| Bit(s) | Field | Type | Reset | Description |
|--------|-------|------|-------|-------------|
| 0 | SS | RW | 0 | Slave Select control |
| 1 | Enable | RW | 0 | SPI controller enable |
| 2 | RX_EN | RW | 0 | Receive enable |
| 31:3 | Reserved | - | 0 | Reserved |

### STATUS Register (Offset: 0x0014)
Status register for SPI controller state.

| Bit(s) | Field | Type | Reset | Description |
|--------|-------|------|-------|-------------|
| 0 | TIP | R | 0 | Transfer in progress |
| 1 | TX_EMPTY | R | 1 | Transmit FIFO empty |
| 2 | TX_FULL | R | 0 | Transmit FIFO full |
| 3 | RX_EMPTY | R | 1 | Receive FIFO empty |
| 4 | RX_FULL | R | 0 | Receive FIFO full |
| 31:5 | Reserved | - | 0 | Reserved |

## Example Register Access

### SPI0 Register Access
```c
// SPI0 Base Address
#define SPI0_BASE 0x30000000

// Write to SPI0 TX Data register
*(volatile uint32_t*)(SPI0_BASE + 0x0004) = 0xAA;

// Read from SPI0 RX Data register
uint32_t rx_data = *(volatile uint32_t*)(SPI0_BASE + 0x0000);

// Configure SPI0 (CPOL=0, CPHA=1)
*(volatile uint32_t*)(SPI0_BASE + 0x0008) = 0x02;
```

### SPI1 Register Access
```c
// SPI1 Base Address
#define SPI1_BASE 0x30010000

// Write to SPI1 TX Data register
*(volatile uint32_t*)(SPI1_BASE + 0x0004) = 0x55;

// Read from SPI1 RX Data register
uint32_t rx_data = *(volatile uint32_t*)(SPI1_BASE + 0x0000);
```

## Wishbone B4 Protocol

All registers are accessed via Wishbone B4 (classic) 32-bit slave interface:
- Word-aligned addresses (32-bit access)
- Little-endian byte ordering
- Single-cycle read latency with ACK
- Byte lane writes supported via wbs_sel_i

## Notes

1. All registers are 32-bit wide and must be accessed on word-aligned addresses
2. The PR (Prescaler) register must be programmed with a value >= 2
3. GCLK register should be set to 1 to enable the SPI controller clock before use
4. Unused address spaces return 0xDEADBEEF on reads and acknowledge but discard writes
5. FIFOs should be flushed before starting a new transaction sequence
