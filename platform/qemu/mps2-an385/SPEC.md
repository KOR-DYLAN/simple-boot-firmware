# QEMU MPS2 AN385 specification

This profile selects the MPS2 AN385 FPGA image in QEMU 8.2.2.

| Property | Configuration |
| --- | --- |
| CPU | Cortex-M3, ARMv7-M, Thumb-2 |
| CPU count / engine | 1 / TCG |
| Endianness | Little-endian |
| Reset mechanism | Initial MSP and reset-handler pointer from address zero |
| C execution state | Privileged Thread mode using MSP |
| System/UART clock | 25 MHz |
| External interrupts | 32 NVIC inputs |
| Vector table | 16 core entries + 32 external IRQ entries; 256-byte alignment |
| Console | CMSDK APB UART0 at `0x40004000` |
| Console configuration | 115200 baud, polling transmit |
| Image loading | `-kernel boot.elf` |
| Floating-point use | Soft-float ABI |

| Memory | Address range | Boot use |
| --- | --- | --- |
| ZBT SSRAM1 | `[0x00000000, 0x00400000)` | Code, vectors, RO, data initializers |
| ZBT SSRAM2/3 | `[0x20000000, 0x20400000)` | RW data, BSS, stack |
| PSRAM | `[0x21000000, 0x22000000)` | Unused |
| Block RAM | `[0x01000000, 0x01004000)` | Unused |

The model also provides RAM aliases. The boot image uses the primary ranges
above, with 64 KiB each for code, RO, RW, and stack. SRAM at address zero is not
physical flash. VTOR is set to the linked vector table. Interrupts stay masked;
no MPU setup is performed.

CPU selection, clocks, memory ranges, and IRQ count are defined by the
[QEMU 8.2.2 MPS2 implementation](https://github.com/qemu/qemu/blob/v8.2.2/hw/arm/mps2.c).
