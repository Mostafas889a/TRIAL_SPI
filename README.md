# TRIAL_SPI - Caravel User Project Integration

## Initial User Prompt
**Request**: Integrate a custom user project into the Caravel SoC with the following peripherals: 2 x SPI

## Project Overview
This project integrates two SPI (Serial Peripheral Interface) controllers into the Caravel SoC as a custom user project. The integration follows Caravel's Wishbone B4 (classic) protocol and includes comprehensive verification using caravel-cocotb.

## Objectives
1. Integrate 2 x SPI peripherals from existing IP cores (CF_SPI)
2. Implement Wishbone B4 (classic) address decoding and multiplexing
3. Create user_project_wrapper module for Caravel integration
4. Develop comprehensive cocotb verification tests
5. Verify RTL functionality using caravel-cocotb
6. Document register map, pad assignments, and integration notes

## Project Requirements
- **Peripherals**: 2 x SPI controllers
- **Protocol**: Wishbone B4 (classic) 32-bit slave
- **Address Space**: Starting at 0x3000_0000
  - SPI0: 0x3000_0000 - 0x3000_FFFF
  - SPI1: 0x3001_0000 - 0x3001_FFFF
- **Integration**: Use existing CF_SPI IP cores with Wishbone wrappers
- **Verification**: cocotb-based tests with caravel-cocotb framework

## Design Approach
1. Use pre-verified CF_SPI IP cores from /nc/ip
2. Link IP cores using ipm_linker tool
3. Create Wishbone address decoder for 2 peripherals
4. Implement user_project module with proper Wishbone handshaking
5. Create user_project_wrapper for Caravel integration
6. Develop firmware drivers and test cases
7. Run comprehensive verification before synthesis

## Project Structure
```
TRIAL_SPI/
├── rtl/
│   ├── user_project.v          # Main Wishbone slave with address decoder
│   └── user_project_wrapper.v  # Caravel wrapper
├── verilog/
│   ├── includes/               # File lists
│   └── dv/cocotb/             # Verification tests
├── ip/                         # Linked IP cores
├── fw/                         # Firmware headers and tests
├── docs/                       # Documentation
│   ├── register_map.md
│   ├── pad_map.md
│   └── integration_notes.md
└── openlane/                   # Synthesis configuration
```

## Current Status
- **Phase**: Project Setup
- **Progress**: Initializing project structure

## Next Steps
1. Copy Caravel user project template
2. Link CF_SPI IP cores using ipm_linker
3. Create documentation (register_map.md, pad_map.md, integration_notes.md)
4. Develop user_project.v with Wishbone decoder
5. Create user_project_wrapper.v
6. Develop firmware and verification tests

---
*Last Updated*: 2025-11-01
*Status*: In Progress