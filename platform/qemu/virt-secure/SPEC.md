# QEMU virt secure-reset specification

This profile uses the CPU, console, main RAM size, and GICv2 selection of
[virt](../virt/SPEC.md), with TrustZone enabled and firmware loaded into secure
flash. `-bios firmware.bin` starts bootloader1 at reset address `0x00000000`.

| Property | AArch64 | AArch32 |
| --- | --- | --- |
| CPU model | Cortex-A53 | Cortex-A15 |
| Reset PC | `0x00000000` | `0x00000000` |
| Hardware reset state | EL3 | Secure SVC |
| Startup/C execution state | EL3h | Monitor mode (`CPSR.M = 0x16`) |
| Security extensions | Enabled | Enabled |
| Virtualization extensions | Enabled | Enabled |
| Bootloader1 initial SP | `0x0e040000` | `0x0e040000` |
| Bootloader2 initial SP | `0x0e050000` | `0x0e050000` |

ARMv7-A names its security monitor **Monitor mode**, rather than an AArch64
exception level. Startup explicitly switches Secure SVC to Monitor mode and
installs MVBAR. AArch64 installs VBAR_EL3 and retains EL3h. This profile does not
transition to the nonsecure world.

| Memory | Address range | Use |
| --- | --- | --- |
| Secure flash bank | `[0x00000000, 0x02000000)` | Reset code, vectors, RO, data initializers |
| Second flash bank | `[0x02000000, 0x04000000)` | Unused |
| Secure SRAM | `[0x0e000000, 0x0f000000)` | RW data, BSS, and stack |
| Main RAM | `[0x40000000, 0x48000000)` | Present, unused by the boot image |

Bootloader1 allocations are code `[0, 0x10000)`, RO
`[0x10000, 0x20000)`, RW `[0x0e000000, 0x0e010000)`, and stack
`[0x0e030000, 0x0e040000)`. Bootloader2 uses code/RO
`[0x00100000, 0x00120000)`, RW `[0x0e010000, 0x0e020000)`, and stack
`[0x0e040000, 0x0e050000)`.
The assembly runtime copies `.data` from its flash LMA to secure SRAM, then
clears BSS and the stack before the first C instruction in each stage. The
combined firmware image retains the linked gap between the two flash images.

The console uses the PL011 at `0x09000000`, accessible from secure state, with
a 24 MHz reference clock and 115200-8-N-1 polling output. GICv2 is present but
interrupts remain masked. MMU and caches remain disabled.

Secure flash visibility and the 16 MiB secure SRAM mapping are defined in the
[QEMU 8.2.2 virt implementation](https://github.com/qemu/qemu/blob/v8.2.2/hw/arm/virt.c).
