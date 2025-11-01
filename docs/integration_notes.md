# Integration Notes - TRIAL_SPI

## Overview
This document provides technical details for integrating the TRIAL_SPI user project into the Caravel SoC, including clock/reset architecture, bus timing, simulation setup, and design constraints.

## Clock and Reset Architecture

### Clock Domain
- **Single Clock Domain Design**: The entire user project operates on a single clock domain
- **Clock Source**: `wb_clk_i` from Caravel SoC
- **Typical Frequency**: 10-50 MHz (configurable)
- **No Clock Gating**: Clock gating is disabled for simplicity and compatibility

### Reset Strategy
- **Reset Type**: Synchronous, active-high reset
- **Reset Signal**: `wb_rst_i` from Caravel SoC
- **Reset Assertion**: All registers reset to known states
- **Reset Duration**: Minimum 2 clock cycles required

### Clock/Reset Connections
```verilog
// In user_project_wrapper.v
module user_project_wrapper (
    input wb_clk_i,      // System clock
    input wb_rst_i,      // Synchronous reset (active high)
    // ... other signals
);

// Both SPI controllers use the same clock and reset
user_project mprj (
    .wb_clk_i(wb_clk_i),
    .wb_rst_i(wb_rst_i),
    // ...
);
```

## Wishbone B4 Bus Integration

### Bus Protocol Summary
- **Protocol**: Wishbone B4 (classic)
- **Data Width**: 32 bits
- **Address Width**: 32 bits
- **Byte Enable**: Supported via wbs_sel_i[3:0]
- **Cycle Type**: Single cycle per transaction

### Timing Specifications

#### Read Cycle Timing
```
Clock:     __|‾‾|__|‾‾|__|‾‾|__|‾‾|__
cyc_i:     _____|‾‾‾‾‾‾‾‾‾‾|_________
stb_i:     _____|‾‾‾‾‾‾‾‾‾‾|_________
we_i:      _________________________
adr_i:     ===XX[ADDR]XXXXXXXXXX====
dat_o:     ===========[DATA]========
ack_o:     __________|‾‾|___________
                      ^
                   1 cycle latency
```

#### Write Cycle Timing
```
Clock:     __|‾‾|__|‾‾|__|‾‾|__|‾‾|__
cyc_i:     _____|‾‾‾‾‾‾‾‾‾‾|_________
stb_i:     _____|‾‾‾‾‾‾‾‾‾‾|_________
we_i:      _____|‾‾‾‾‾‾‾‾‾‾|_________
adr_i:     ===XX[ADDR]XXXXXXXXXX====
dat_i:     ===XX[DATA]XXXXXXXXXX====
sel_i:     ===XX[MASK]XXXXXXXXXX====
ack_o:     __________|‾‾|___________
                      ^
                   1 cycle latency
```

### Address Decoding

The user_project implements address decoding for 2 SPI peripherals using bits [19:16]:

```verilog
// Address decode for 2 peripherals (64KB windows)
wire [1:0] periph_sel = wbs_adr_i[16];  // Bit 16 selects between SPI0/SPI1
wire [1:0] stb;

assign stb[0] = ~periph_sel & wbs_stb_i;  // SPI0: 0x3000_0000
assign stb[1] = periph_sel & wbs_stb_i;   // SPI1: 0x3001_0000
```

### Wishbone Signal Routing

**Critical Rule**: Never gate or qualify `wbs_cyc_i` with address decoding!

```verilog
// CORRECT: Route cyc_i unmodified to all peripherals
CF_SPI_WB spi0 (
    .cyc_i(wbs_cyc_i),        // Unmodified
    .stb_i(stb[0]),           // Gated with address decode
    // ...
);

CF_SPI_WB spi1 (
    .cyc_i(wbs_cyc_i),        // Unmodified
    .stb_i(stb[1]),           // Gated with address decode
    // ...
);
```

### Response Multiplexing
```verilog
// ACK: OR all peripheral ACKs
assign wbs_ack_o = spi0_ack | spi1_ack;

// Read Data: Mux based on peripheral select
reg [31:0] wbs_dat_o;
always @(*) begin
    case (periph_sel)
        1'b0: wbs_dat_o = spi0_dat_o;  // SPI0
        1'b1: wbs_dat_o = spi1_dat_o;  // SPI1
        default: wbs_dat_o = 32'hDEADBEEF;  // Invalid address
    endcase
end
```

## Power Domains

### User Project Power
- **Power Domain**: VCCD2/VSSD2
- **Voltage**: 1.8V (typical)
- **Power Pins**: Connected via user_project_wrapper

### Power Pin Connections
```verilog
`ifdef USE_POWER_PINS
    .VPWR(vccd2),    // 1.8V supply
    .VGND(vssd2),    // Ground
`endif
```

## Interrupt Architecture

### IRQ Mapping
```verilog
assign user_irq[0] = spi0_irq;  // SPI0 interrupts
assign user_irq[1] = spi1_irq;  // SPI1 interrupts
assign user_irq[2] = 1'b0;      // Reserved
```

### IRQ Protocol
- **Type**: Level-sensitive (active high)
- **Assertion**: Raised when interrupt condition occurs
- **Clearing**: Write-one-to-clear via IC register
- **Masking**: Controlled via IM register per peripheral

### Interrupt Sources (per SPI)
- TX FIFO threshold
- RX FIFO threshold
- Transfer complete
- Overrun/underrun errors

## IP Core Integration Details

### CF_SPI IP Version
- **IP Name**: CF_SPI
- **Version**: v2.0.1
- **Wrapper**: CF_SPI_WB (Wishbone)
- **Dependencies**: CF_IP_UTIL v1.0.0

### IP Parameters
```verilog
CF_SPI_WB #(
    .CDW(8),    // Clock divider width
    .FAW(4)     // FIFO address width (16 deep)
) spi_inst (
    // connections
);
```

### IP File Locations
- **IP Root**: `/workspace/TRIAL_SPI/ip/CF_SPI/`
- **RTL Core**: `hdl/rtl/CF_SPI.v`
- **Wishbone Wrapper**: `hdl/rtl/bus_wrappers/CF_SPI_WB.v`
- **Documentation**: `CF_SPI.pdf`, `README.md`

## Simulation and Verification

### Caravel-Cocotb Framework

#### Prerequisites
- Python 3.8+
- cocotb
- caravel_cocotb package
- RISC-V GCC toolchain for firmware compilation

#### Test Structure
```
verilog/dv/cocotb/
├── spi0_test/
│   ├── spi0_test.py        # Cocotb test
│   ├── spi0_test.c         # Firmware
│   └── spi0_test.hex       # Compiled firmware
├── spi1_test/
│   ├── spi1_test.py
│   ├── spi1_test.c
│   └── spi1_test.hex
├── system_test/
│   ├── system_test.py      # System integration test
│   ├── system_test.c
│   └── system_test.hex
├── cocotb_tests.py         # Test runner
└── design_info.yaml        # Design configuration
```

#### Running Tests
```bash
# Run all tests
cd /workspace/TRIAL_SPI/verilog/dv/cocotb
python cocotb_tests.py

# Run specific test
python cocotb_tests.py -test spi0_test

# Generate waveforms
python cocotb_tests.py -wave
```

#### design_info.yaml Configuration
```yaml
design_name: user_project_wrapper
top_level: user_project_wrapper
rtl_files:
  - ../../rtl/user_project.v
  - ../../rtl/user_project_wrapper.v
  - ../../ip/CF_SPI/hdl/rtl/CF_SPI.v
  - ../../ip/CF_SPI/hdl/rtl/bus_wrappers/CF_SPI_WB.v
  - ../../ip/CF_IP_UTIL/hdl/rtl/*.v
parameters: {}
```

### Firmware Development

#### Firmware APIs
Located in `fw/spi_regs.h`:
```c
#define SPI0_BASE 0x30000000
#define SPI1_BASE 0x30010000

#define SPI_RXDATA_OFFSET   0x0000
#define SPI_TXDATA_OFFSET   0x0004
#define SPI_CFG_OFFSET      0x0008
#define SPI_CTRL_OFFSET     0x000C
#define SPI_PR_OFFSET       0x0010
#define SPI_STATUS_OFFSET   0x0014

// Helper macros
#define SPI_REG(base, offset) (*(volatile uint32_t*)((base) + (offset)))
```

#### Example Firmware Usage
```c
#include "spi_regs.h"

void spi0_init(void) {
    // Enable clock gate
    SPI_REG(SPI0_BASE, 0xFF10) = 0x1;
    
    // Set prescaler (divide by 4)
    SPI_REG(SPI0_BASE, SPI_PR_OFFSET) = 0x04;
    
    // Configure: CPOL=0, CPHA=0
    SPI_REG(SPI0_BASE, SPI_CFG_OFFSET) = 0x00;
    
    // Enable SPI and RX
    SPI_REG(SPI0_BASE, SPI_CTRL_OFFSET) = 0x06;
}

void spi0_write_byte(uint8_t data) {
    SPI_REG(SPI0_BASE, SPI_TXDATA_OFFSET) = data;
}

uint8_t spi0_read_byte(void) {
    return SPI_REG(SPI0_BASE, SPI_RXDATA_OFFSET) & 0xFF;
}
```

## Synthesis and Place & Route

### Yosys Synthesis

#### Synthesis Script (syn/yosys.ys)
```tcl
read_verilog rtl/user_project.v
read_verilog ip/CF_SPI/hdl/rtl/CF_SPI.v
read_verilog ip/CF_SPI/hdl/rtl/bus_wrappers/CF_SPI_WB.v
read_verilog ip/CF_IP_UTIL/hdl/rtl/*.v

hierarchy -check -top user_project
synth -top user_project
dfflibmap -liberty sky130_fd_sc_hd__tt_025C_1v80.lib
abc -liberty sky130_fd_sc_hd__tt_025C_1v80.lib
clean

stat
check
write_verilog -noattr user_project_synth.v
```

#### Running Synthesis
```bash
cd /workspace/TRIAL_SPI
yosys syn/yosys.ys
```

#### Expected Results
- No inferred latches
- All combinational loops resolved
- Clean synthesis (no warnings about missing signals)

### Verilator Lint

```bash
# Lint user_project.v
verilator --lint-only --Wall --Wno-EOFNEWLINE \
    -I./ip/CF_SPI/hdl/rtl \
    -I./ip/CF_IP_UTIL/hdl/rtl \
    rtl/user_project.v
```

### OpenLane Hardening

**Note**: OpenLane hardening should only be performed after successful verification!

#### user_project Macro Configuration
File: `openlane/user_project/config.json`

Key settings:
- `DESIGN_NAME`: "user_project"
- `CLOCK_PORT`: "wb_clk_i"
- `CLOCK_PERIOD`: 25 (40 MHz)
- `VERILOG_FILES`: List all RTL and IP files
- `DIE_AREA`: Based on utilization

#### user_project_wrapper Configuration
File: `openlane/user_project_wrapper/config.json`

Includes macro placement:
```json
{
    "MACROS": {
        "user_project": {
            "instances": {
                "mprj": {
                    "location": [1500, 1500],
                    "orientation": "N"
                }
            }
        }
    }
}
```

## Design Constraints

### Timing Constraints
- **Target Frequency**: 40 MHz (25ns period)
- **Setup Time Margin**: 10% (2.5ns)
- **Hold Time Margin**: Positive hold check required
- **Clock Uncertainty**: 0.25ns (jitter + skew)

### Area Constraints
- **User Project Area**: ~3000um x 3600um
- **Estimated Utilization**: 20-30%
- **SPI Controller Area**: ~500um² per instance

### Power Constraints
- **Target Power**: < 10mW @ 40 MHz
- **Leakage**: < 1mW
- **Dynamic Power**: Depends on SPI activity

## Common Integration Issues

### Issue 1: Wishbone Bus Hangs
**Symptom**: System hangs during register access
**Cause**: `wbs_cyc_i` gated with address decode
**Solution**: Route `wbs_cyc_i` unmodified to all peripherals

### Issue 2: Invalid Address Reads Hang
**Symptom**: Reads to invalid addresses don't return
**Cause**: No ACK generated for out-of-range addresses
**Solution**: Generate ACK with 0xDEADBEEF for invalid addresses

### Issue 3: Data Corruption on Reads
**Symptom**: Read data is incorrect or unstable
**Cause**: Multiple peripherals driving dat_o simultaneously
**Solution**: Ensure one-hot peripheral selection and proper muxing

### Issue 4: Interrupt Storms
**Symptom**: Continuous interrupts
**Cause**: Interrupts not cleared properly
**Solution**: Write 1 to IC register to clear interrupts

### Issue 5: SPI Clock Not Running
**Symptom**: No SCLK output
**Cause**: GCLK register not set to enable clock
**Solution**: Write 1 to GCLK register (offset 0xFF10) before operation

## Verification Checklist

Before proceeding to synthesis:
- [ ] All Wishbone transactions complete with ACK
- [ ] Invalid addresses return 0xDEADBEEF
- [ ] Both SPI controllers accessible independently
- [ ] SPI MOSI/MISO/CSB/SCLK signals toggle correctly
- [ ] Interrupts assert and clear properly
- [ ] No Verilator lint errors
- [ ] No Yosys synthesis warnings
- [ ] Firmware can read/write all registers
- [ ] FIFO operations work correctly
- [ ] Multiple consecutive transactions work
- [ ] System integration test passes

## References

### Documentation
- Caravel User Project Wrapper: [Efabless Caravel Docs](https://caravel-harness.readthedocs.io/)
- Wishbone B4 Specification: [OpenCores Wishbone](https://opencores.org/howto/wishbone)
- CF_SPI IP Documentation: `/workspace/TRIAL_SPI/ip/CF_SPI/CF_SPI.pdf`

### Tools
- OpenLane: https://github.com/efabless/openlane2
- Caravel-Cocotb: https://github.com/efabless/caravel-cocotb
- Yosys: https://yosyshq.net/yosys/

## Support and Contact

For issues related to:
- **CF_SPI IP**: See IP documentation and LICENSE
- **Caravel Integration**: Efabless community forums
- **This Project**: See repository issues

---
*Document Version*: 1.0  
*Last Updated*: 2025-11-01  
*Author*: NativeChips RTL Agent
