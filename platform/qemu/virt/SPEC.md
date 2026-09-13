# QEMU virt specification

This profile describes the configuration selected by `platform.cmake`, with
QEMU 8.2.2 as the reference implementation. It models a virtual board, not a
physical development board.

| Property | AArch64 | AArch32 |
| --- | --- | --- |
| CPU model | Cortex-A53 | Cortex-A15 |
| Architecture | ARMv8-A, AArch64 | ARMv7-A, ARM state |
| CPU count | 1 | 1 |
| Execution engine | TCG | TCG |
| Endianness | Little-endian | Little-endian |
| Reset entry state | EL2 | HYP |
| C execution state | EL1h | SVC |
| Security extensions | Disabled in this profile | Disabled in this profile |
| Virtualization extensions | Enabled | Enabled |
| Interrupt controller | GICv2; interrupts remain masked | GICv2; interrupts remain masked |
| Floating-point/SIMD use | General registers only | Soft-float ABI |

The ELF generic loader applies the entry point and physical load addresses.
RAM capacity is 128 MiB. The first 2 MiB is reserved for the DTB; code starts at
`0x40200000`. The program leaves MMU/caches disabled and uses polling UART output.
These execution settings are defined in [platform.cmake](platform.cmake).

| Region or device | Address range / setting |
| --- | --- |
| Main RAM | `[0x40000000, 0x48000000)` |
| Flash banks | `[0x00000000, 0x04000000)`; unused by this boot profile |
| GIC distributor | `0x08000000` |
| GIC CPU interface | `0x08010000` |
| Console PL011 | `0x09000000` |
| UART reference clock | 24 MHz |
| UART configuration | 115200 baud, 8 data bits, no parity, 1 stop bit |
| Default code / RO / RW / stack capacity | 64 KiB each |

Device layout and clock values follow the
[QEMU 8.2.2 virt implementation](https://github.com/qemu/qemu/blob/v8.2.2/hw/arm/virt.c).
Security and virtualization switches are described by the
[QEMU virt documentation](https://www.qemu.org/docs/master/system/arm/virt.html).
The emulation is not a cycle-accurate CPU performance specification.
