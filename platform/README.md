# Platform guide

All emulator names, CPU model selections, machine arguments, device addresses,
and external interrupt counts live under `platform/`. Generic code consumes
platform configuration without selecting a machine.

## Included platforms

| `BOOT_PLATFORM` | Architectures | CPU | Console |
| --- | --- | --- | --- |
| `qemu/virt` | `aarch64`, `aarch32` | Cortex-A53 / Cortex-A15 | PL011 |
| `qemu/virt-secure` | `aarch64`, `aarch32` | Cortex-A53 / Cortex-A15 | PL011 |
| `qemu/mps2-an385` | `cortex-m` | Cortex-M3 | CMSDK APB UART |

The `virt-secure` board provides `aarch64_defconfig` and `aarch32_defconfig`;
MPS2 AN385 provides `cortex-m_defconfig`. CMake discovers the single matching
default file for the selected architecture and reads the platform name from its
`CONFIG_PLATFORM_NAME` value. No common file contains a default board list.
When `BOOT_PLATFORM` is explicit, `BOOT_CONFIG` names a file directly under that
board's `configs/` directory and must not contain directory components. Multiple
comma-separated names are merged from left to right.

Install QEMU on Ubuntu/Debian with:

```sh
sudo apt install qemu-system-arm
```

CMake searches for `qemu-system-aarch64` or `qemu-system-arm` according to the
selected platform. Reconfigure after installing the executable.

```sh
make ARCH=aarch64
make run ARCH=aarch64
make ARCH=cortex-m PLATFORM=qemu/mps2-an385
```

## Machine specifications

- [virt RAM-boot specification](qemu/virt/SPEC.md)
- [virt secure-reset specification](qemu/virt-secure/SPEC.md)
- [MPS2 AN385 specification](qemu/mps2-an385/SPEC.md)

## Secure reset at address zero

Address-zero secure firmware boot is the default for AArch64 and AArch32:

```sh
make ARCH=aarch64
make run ARCH=aarch64

make ARCH=aarch32
make run ARCH=aarch32
```

Replace `run` with `debug` and connect with `gdb` using the same variables.
Both targets reset with PC zero. AArch64 remains in EL3h; AArch32 switches from
Secure SVC to Monitor mode before runtime initialization. Firmware is loaded
with `-bios firmware.bin`, using secure flash for both bootloader stages and
secure SRAM for their independent RW and stack regions.

To select RAM boot explicitly, use a separate build directory:

```sh
make run ARCH=aarch64 PLATFORM=qemu/virt BUILD_DIR=build/ram-aarch64
make run ARCH=aarch32 PLATFORM=qemu/virt BUILD_DIR=build/ram-aarch32
```

CMake preserves the config and platform selection in each build directory. Use
`make distclean ARCH=<architecture>` before configuring that directory with
the default selection if its cache selects another platform.

## Execution and debugging

[qemu/targets.cmake](qemu/targets.cmake) defines the QEMU `run` and `debug`
targets. `run` starts execution; `debug` adds `-S` and a GDB server bound to
`127.0.0.1:${BOOT_GDB_PORT}`. UART output uses the terminal. Exit with **Ctrl+C**.
The common `gdb` target connects to this server. QEMU loads the selected ELF or
firmware binary before GDB connects, so no separate GDB `load` is needed.

The `virt` platform uses TCG, one CPU, 128 MiB RAM, virtualization, and GICv2.
The generic loader applies each ELF's physical addresses. MPS2 uses
`-kernel bootloader1.elf`, loads `bootloader2.elf` separately, and resets through
the bootloader1 vector table at address zero.

## Memory configuration

Bootloader1 retains the reset layout:

| Macro | `qemu/virt-secure` | `qemu/virt` | `qemu/mps2-an385` |
| --- | --- | --- | --- |
| `CODE_START` | `0x00000000` | `0x40200000` | `0x00000000` |
| `CODE_END` | `0x00010000` | `0x40210000` | `0x00010000` |
| `RO_DATA_START` | `0x00010000` | `0x40210000` | `0x00010000` |
| `RO_DATA_END` | `0x00020000` | `0x40220000` | `0x00020000` |
| `RW_DATA_START` | `0x0E000000` | `0x40400000` | `0x20000000` |
| `RW_DATA_END` | `0x0E010000` | `0x40410000` | `0x20010000` |
| `STACK_END` | `0x0E030000` | `0x407F0000` | `0x20030000` |
| `STACK_START` | `0x0E040000` | `0x40800000` | `0x20040000` |

Bootloader2 uses separate code, RW, and stack allocations:

| Region | `qemu/virt-secure` | `qemu/virt` | `qemu/mps2-an385` |
| --- | --- | --- | --- |
| Code / RO | `[0x00100000, 0x00120000)` | `[0x41000000, 0x41020000)` | `[0x00100000, 0x00120000)` |
| RW | `[0x0E010000, 0x0E020000)` | `[0x41200000, 0x41210000)` | `[0x20100000, 0x20110000)` |
| Stack | `[0x0E040000, 0x0E050000)` | `[0x415F0000, 0x41600000)` | `[0x20130000, 0x20140000)` |

Memory boundaries and permitted ranges are defined in:

- [virt secure memory header](qemu/virt-secure/include/platform_memory.h)
- [virt memory header](qemu/virt/include/platform_memory.h)
- [MPS2 memory header](qemu/mps2-an385/include/platform_memory.h)

`virt` reserves the first 2 MiB of RAM for the DTB and places the image within
`[0x40000000, 0x48000000)`. MPS2 places code/RO in ZBT SSRAM at
`[0x00000000, 0x00400000)` and RW/stack in `[0x20000000, 0x20400000)`.
Its reset vector address is fixed at zero.

The MPS2 `platform_def.h` supplies 32 external IRQ entries and 256-byte vector
alignment. Common startup combines these with the architecture's core vectors
and verifies the resulting table size and alignment.

## Platform contract

A platform directory contains:

- `KConfig`: board selection and board-specific configuration symbols.
- `configs/<architecture>_defconfig`: default boolean, string, and numeric
  settings for an architecture.
- `configs/<architecture>.config`: settings selected when the board is explicit.
- `platform.cmake`: supported architectures, CPU flags, console driver, and
  optional `BOOT_PLATFORM_TARGETS` execution-hook path.
- `CMakeLists.txt`: sources contributed to the aggregate `libdriver` target.
- `console.c`: console device binding.
- `include/platform_def.h`: device parameters and external IRQ configuration.
- `include/platform_memory.h`: memory boundaries and permitted ranges.

For the QEMU backend, `platform.cmake` also sets `BOOT_QEMU_NAME`,
`BOOT_QEMU_ARGS`, and `BOOT_QEMU_LOAD_MODE` (`loader`, `kernel`, or `bios`). Other
platforms can supply their own execution hook without depending on QEMU.
