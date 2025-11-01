from caravel_cocotb.caravel_interfaces import test_configure, report_test
import cocotb
from cocotb.triggers import ClockCycles, RisingEdge, FallingEdge

async def wait_for_pulses(env, count, label):
    for _ in range(count):
        await env.wait_mgmt_gpio(1)
        await env.wait_mgmt_gpio(0)
    cocotb.log.info(f"[TEST] {label}")

async def monitor_spi_transaction(dut, env, expected_bytes, label):
    received_bytes = []
    for expected_byte in expected_bytes:
        byte_bits = []
        for bit_idx in range(8):
            await RisingEdge(dut.uut.mprj.spi_sclk[0])
            mosi_bit = dut.uut.mprj.spi_mosi[0].value.integer
            byte_bits.append(mosi_bit)
        
        received_byte = 0
        for i, bit in enumerate(byte_bits):
            received_byte |= (bit << (7 - i))
        
        received_bytes.append(received_byte)
        cocotb.log.info(f"[TEST] {label}: Received byte 0x{received_byte:02X}")
        
        if received_byte != expected_byte:
            cocotb.log.error(f"[TEST] {label}: Expected 0x{expected_byte:02X}, got 0x{received_byte:02X}")
            return False
    
    cocotb.log.info(f"[TEST] {label}: All bytes matched!")
    return True

@cocotb.test()
@report_test
async def spi0_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=5000000)
    cocotb.log.info("[TEST] Start spi0_test")
    
    await caravelEnv.release_csb()
    
    cocotb.log.info("[TEST] Wait for firmware ready")
    await wait_for_pulses(caravelEnv, 1, "firmware ready")
    
    cocotb.log.info("[TEST] Wait for SPI0 enabled")
    await wait_for_pulses(caravelEnv, 1, "SPI0 enabled")
    
    csb_value = dut.uut.mprj.spi_csb[0].value.integer
    cocotb.log.info(f"[TEST] CSB before first byte: {csb_value}")
    
    cocotb.log.info("[TEST] Monitoring first SPI transaction (0xA5)")
    cocotb.start_soon(wait_for_pulses(caravelEnv, 1, "first byte transmitted"))
    result1 = await monitor_spi_transaction(dut, caravelEnv, [0xA5], "first byte")
    
    if not result1:
        cocotb.log.error("[TEST] First byte transmission failed!")
    
    cocotb.log.info("[TEST] Monitoring second SPI transaction (0x5A)")
    cocotb.start_soon(wait_for_pulses(caravelEnv, 1, "second byte transmitted"))
    result2 = await monitor_spi_transaction(dut, caravelEnv, [0x5A], "second byte")
    
    if not result2:
        cocotb.log.error("[TEST] Second byte transmission failed!")
    
    cocotb.log.info("[TEST] Wait for SPI0 disabled")
    await wait_for_pulses(caravelEnv, 1, "SPI0 disabled")
    
    csb_value = dut.uut.mprj.spi_csb[0].value.integer
    cocotb.log.info(f"[TEST] CSB after disable: {csb_value}")
    
    if result1 and result2:
        cocotb.log.info("[TEST] SPI0 test PASSED!")
    else:
        cocotb.log.error("[TEST] SPI0 test FAILED!")
    
    cocotb.log.info("[TEST] End spi0_test")
