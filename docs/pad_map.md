# Pad Map - TRIAL_SPI

## Overview
This document describes the GPIO pad assignments for the TRIAL_SPI Caravel user project, which integrates 2 x SPI controllers.

## Caravel GPIO Overview
- Caravel provides 38 user GPIOs (mprj_io[37:0])
- GPIOs 0-4 are typically reserved for specific functions
- This project uses GPIOs 5-12 for SPI peripherals

## SPI Signal Overview
Each SPI controller requires 4 signals:
- **MISO** (Master In Slave Out): Input from slave device
- **MOSI** (Master Out Slave In): Output to slave device
- **CSB** (Chip Select Bar): Output, active-low chip select
- **SCLK** (Serial Clock): Output clock signal

## GPIO Pad Assignments

### Default Pin Mapping

| GPIO Pin | Direction | Signal Name | Peripheral | Description |
|----------|-----------|-------------|------------|-------------|
| 5 | Output | spi0_mosi | SPI0 | Master Out Slave In |
| 6 | Input | spi0_miso | SPI0 | Master In Slave Out |
| 7 | Output | spi0_csb | SPI0 | Chip Select Bar (active low) |
| 8 | Output | spi0_sclk | SPI0 | Serial Clock |
| 9 | Output | spi1_mosi | SPI1 | Master Out Slave In |
| 10 | Input | spi1_miso | SPI1 | Master In Slave Out |
| 11 | Output | spi1_csb | SPI1 | Chip Select Bar (active low) |
| 12 | Output | spi1_sclk | SPI1 | Serial Clock |

### Reserved/Unused Pins
- **GPIO[4:0]**: Reserved for system use (avoid using)
- **GPIO[37:13]**: Available for future expansion

## Pad Configuration Details

### SPI0 Pads (GPIO 5-8)

#### GPIO[5] - spi0_mosi (Output)
```verilog
assign mprj_io_out[5] = spi0_mosi;
assign mprj_io_oeb[5] = 1'b0;  // Output enable (active low)
```

#### GPIO[6] - spi0_miso (Input)
```verilog
assign spi0_miso = mprj_io_in[6];
assign mprj_io_out[6] = 1'b0;  // Tie output low
assign mprj_io_oeb[6] = 1'b1;  // Disable output (input mode)
```

#### GPIO[7] - spi0_csb (Output)
```verilog
assign mprj_io_out[7] = spi0_csb;
assign mprj_io_oeb[7] = 1'b0;  // Output enable (active low)
```

#### GPIO[8] - spi0_sclk (Output)
```verilog
assign mprj_io_out[8] = spi0_sclk;
assign mprj_io_oeb[8] = 1'b0;  // Output enable (active low)
```

### SPI1 Pads (GPIO 9-12)

#### GPIO[9] - spi1_mosi (Output)
```verilog
assign mprj_io_out[9] = spi1_mosi;
assign mprj_io_oeb[9] = 1'b0;  // Output enable (active low)
```

#### GPIO[10] - spi1_miso (Input)
```verilog
assign spi1_miso = mprj_io_in[10];
assign mprj_io_out[10] = 1'b0;  // Tie output low
assign mprj_io_oeb[10] = 1'b1;  // Disable output (input mode)
```

#### GPIO[11] - spi1_csb (Output)
```verilog
assign mprj_io_out[11] = spi1_csb;
assign mprj_io_oeb[11] = 1'b0;  // Output enable (active low)
```

#### GPIO[12] - spi1_sclk (Output)
```verilog
assign mprj_io_out[12] = spi1_sclk;
assign mprj_io_oeb[12] = 1'b0;  // Output enable (active low)
```

## Interrupt Mapping

The user project has 3 interrupt lines available (user_irq[2:0]):

| IRQ Line | Source | Description |
|----------|--------|-------------|
| user_irq[0] | SPI0 | SPI0 interrupt (FIFO thresholds, transfer complete) |
| user_irq[1] | SPI1 | SPI1 interrupt (FIFO thresholds, transfer complete) |
| user_irq[2] | - | Reserved for future use |

## Pad IO Standards
- All pads use 3.3V CMOS logic levels
- Output drive strength: Default (configured via Caravel system)
- Input threshold: CMOS compatible
- Pull-up/Pull-down: Configured via Caravel management SoC

## Changing Pad Assignments

To modify the pad assignments:

1. Edit `rtl/user_project_wrapper.v`
2. Change the pad index in the wire connections:
   ```verilog
   // Example: Move spi0_mosi from GPIO[5] to GPIO[15]
   assign mprj_io_out[15] = spi0_mosi;
   assign mprj_io_oeb[15] = 1'b0;
   ```
3. Update this documentation
4. Update firmware header files in `fw/` directory
5. Update testbench pad connections in verification tests

## Physical Considerations

### Signal Integrity
- Keep SPI signals on adjacent pins to minimize routing
- SCLK should be routed carefully to minimize skew
- Consider adding series termination resistors for high-speed operation

### ESD Protection
- All pads have built-in ESD protection (Caravel standard)
- Additional external protection may be needed for harsh environments

### Power Domains
- All user project IOs operate from VCCD2/VSSD2 power domain
- Ensure proper power sequencing in multi-voltage systems

## External Interface Example

### Connecting External SPI Flash
```
Caravel GPIO[5] (spi0_mosi) -----> DI (Data In) of SPI Flash
Caravel GPIO[6] (spi0_miso) <----- DO (Data Out) of SPI Flash
Caravel GPIO[7] (spi0_csb)  -----> CS# (Chip Select) of SPI Flash
Caravel GPIO[8] (spi0_sclk) -----> CLK (Clock) of SPI Flash
```

## Testing and Verification

During verification:
- Input pads should be driven from testbench
- Output pads should be monitored in testbench
- Verify correct direction (input vs output) configuration
- Check for any conflicts or shorts on bidirectional pads

## Notes

1. Avoid using GPIO[4:0] unless necessary (reserved for special functions)
2. All SPI outputs are push-pull mode (not open-drain)
3. Chip select (CSB) is active low
4. MISO inputs should have external pull-ups on the slave device
5. Maximum SPI clock frequency depends on the prescaler setting and system clock
6. Default unused GPIOs are configured as inputs with output drivers disabled
