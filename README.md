# simple-boot

Two-stage bare-metal startup for AArch64, AArch32, and ARMv7-M. Build with CMake,
inspect the generated memory maps, and debug with GDB. Platform code owns CPU
model selection, device addresses, external interrupts, and execution backends.
See the [Platform guide](platform/README.md) for supported configurations.

## Quick start

Install GNU Make, CMake 3.21 or later, Ninja, Python 3.6 or later, and a GNU cross
toolchain. Add a GDB that supports the target architecture and the execution
tools described by the selected platform.

```sh
sudo apt install make cmake ninja-build python3 \
    gcc-aarch64-linux-gnu gcc-arm-none-eabi gdb-multiarch

make ARCH=aarch64
make run ARCH=aarch64
```

Use `ARCH=aarch32` or `ARCH=cortex-m` for the other architecture paths. The
platform directory defines defaults when `PLATFORM` is omitted. Bootloader1
initializes its runtime and jumps to bootloader2. Bootloader2 either waits or
jumps to a configured U-Boot or kernel entry point. Initialization failures
print `BOOT FAIL` before halting.

The implementation uses freestanding C11 and GNU assembly. It supplies its own
size, integer, memory, and string declarations and implementations under
`include/library/libc/` and `library/libc/`. Compilation uses `-nostdinc`,
`-ffreestanding`, and `-fno-builtin`; no toolchain libc headers or hosted libc
are used. Startup assumes
little-endian execution with the MMU and caches disabled. Interrupts remain
masked. The included console drivers use polling. The project provides a small
memory/string library, with no OS, heap, C++ runtime, or floating-point/SIMD setup.

## Build commands

| Command | Action |
| --- | --- |
| `make` or `make build` | Configure, compile, and generate all artifacts |
| `make configure` | Configure and select the editor compilation database |
| `make defconfig` | Reset `.config` from the selected board configurations |
| `make menuconfig` | Edit configuration using a terminal menu |
| `make savedefconfig` | Save `BUILD_DIR/defconfig` relative to KConfig defaults |
| `make savefragmentconfig` | Save `BUILD_DIR/fragment.config` relative to board configs |
| `make showconfig` | Print the normalized generated configuration |
| `make run` | Build and invoke the platform execution target |
| `make debug` | Build and invoke the platform debug-server target |
| `make gdb` | Build and connect GDB to the debug server |
| `make layout` | Generate artifacts and print ELF sections/program headers |
| `make target TARGET=liblibrary` | Build the aggregate freestanding library |
| `make targets` | List available CMake targets |
| `make clean` | Remove outputs from an existing build directory |
| `make distclean` | Remove the selected build directory, including CMake configuration |
| `make help` | Show commands and variables |

All build and target-list commands run configure first. Additional CMake cache
settings persist across configure runs.

`make distclean` removes the entire selected `BUILD_DIR`, including its cache,
compilation database, and generated artifacts. It also removes the generated
root `.clangd` if that file selects the same build directory. Other build
directories and their editor selections are preserved. No configure step is
required, and repeating the command succeeds when the directory is already absent.
Existing directories must contain a CMake cache belonging to this source tree;
source directories, their ancestors, and symbolic-link build directories are rejected.

```sh
make distclean ARCH=aarch64
make distclean BUILD_DIR=build/custom-memory
```

| Make variable | Default | Purpose |
| --- | --- | --- |
| `ARCH` | `aarch64` | Target architecture |
| `PLATFORM` | Derived from the default config | Directory under `platform/` |
| `CONFIG` | Selected board config | Comma-separated file names under the board's `configs/` directory |
| `BUILD_DIR` | `build/$(ARCH)` | Build directory |
| `BUILD_TYPE` | `Debug` | `Debug` or `Release` |
| `GENERATOR` | `Ninja` | `Ninja` or `Unix Makefiles` |
| `JOBS` | Build tool default | Parallel compilation jobs |
| `TARGET` | `boot` | Target for `make target` |
| `CMAKE_ARGS` | Empty | Extra configure arguments |
| `BUILD_ARGS` | Empty | Extra build arguments |
| `CMAKE` | `cmake` | CMake executable |

Use separate build directories for different architectures, platforms, toolchains,
or generators:

```sh
make ARCH=aarch64 BUILD_DIR=build/release-aarch64 BUILD_TYPE=Release JOBS=4
```

## KConfig build composition

Every build directory in the source tree contains both `CMakeLists.txt` and
`KConfig`. The root [KConfig](KConfig) sources the architecture, library, driver,
platform, and boot configuration trees. Board configurations use the following
location and assignment forms:

```text
platform/<vendor>/<board>/configs/<name>

CONFIG_FEATURE=y
CONFIG_OTHER_FEATURE=n
CONFIG_PLATFORM_NAME="vendor/board"
CONFIG_GDB_PORT=1234
```

The Python parser supports `bool`, `string`, `int`, `hex`, and unquoted `value`
symbols, defaults, single-symbol dependencies, nested `source` statements, and
the `# CONFIG_NAME is not set` form. It uses only the Python standard library.
Unknown symbols, invalid values, duplicate assignments, unmet dependencies, and
recursive source inclusion stop configuration.

Configuration produces `BUILD_DIR/.config`, `BUILD_DIR/generated/autoconf.h`,
and `BUILD_DIR/generated/config.cmake`. C and assembly compilation, linker-script
preprocessing, and CMake directory composition consume these generated files.
Changes to an input `KConfig` or the selected board configuration automatically
trigger CMake configuration again.

Multiple files are merged from left to right. Later assignments override earlier
assignments, while duplicate assignments within one file remain errors:

```sh
make ARCH=aarch64 PLATFORM=qemu/virt-secure \
    CONFIG='aarch64_defconfig,debug.config'
```

The command above merges both files into `BUILD_DIR/.config`. Commas, semicolons,
and whitespace can separate file names when invoking CMake directly; quoting is
recommended when whitespace is used.

`make menuconfig` starts from the merged `.config`. Arrow keys or `j`/`k` move,
Space or Enter edits a value, `S` saves, and `Q` exits without saving. Saved
changes are recorded in `BUILD_DIR/kconfig.fragment` relative to the selected
board configs and are applied last on subsequent configure runs.

`make defconfig` removes the local menu fragment and reconstructs `.config` from
the files named by `CONFIG`. `make savedefconfig` writes symbols that differ from
KConfig defaults. `make savefragmentconfig` writes symbols that differ from the
merged board configs; `make fragmentconfig` is an alias.

The build helpers conditionally compose every selectable build layer:

```cmake
add_subdirectory_if_enabled(CONFIG_NAME directory)
add_library_if_enabled(CONFIG_NAME target STATIC sources...)
add_executable_if_enabled(CONFIG_NAME target sources...)
add_source_if_enabled(CONFIG_NAME sources...)
target_link_libraries_if_enabled(CONFIG_NAME target PUBLIC libraries...)
```

The directory helper also requires the selected directory to contain both
`CMakeLists.txt` and `KConfig`. `add_library_if_enabled` records the aggregate
target as an inherited directory property, so child directories add sources
without repeating its name. The platform tree selects the existing `libdriver`
aggregate before contributing board bindings.

Each architecture has one default file named `<arch>_defconfig` across the
platform tree. Its location selects the default board, while
`CONFIG_PLATFORM_NAME` inside the file provides the platform path. A board may
also provide `<arch>.config` for explicit `PLATFORM` selection. Select another
configuration explicitly when creating a separate build directory:

```sh
make defconfig ARCH=aarch64 PLATFORM=qemu/virt \
    BUILD_DIR=build/ram-aarch64 \
    CONFIG=aarch64.config
make showconfig ARCH=aarch64 PLATFORM=qemu/virt BUILD_DIR=build/ram-aarch64
```

Each `CONFIG` entry accepts a file name only. CMake resolves it under
`platform/<vendor>/<board>/configs/` after selecting `PLATFORM`; directory
components and path traversal are rejected. When `PLATFORM` is omitted, the
default board's `configs/` directory is used.

### Direct CMake and toolchain selection

Presets use Ninja and Debug. Replace `aarch64` with another architecture as needed:

```sh
cmake --preset aarch64
cmake --build --preset aarch64
cmake --build build/aarch64 --target run
```

CMake selects `cmake/toolchains/<BOOT_ARCH>.cmake`. Shared GNU tool discovery is
in [arm-gcc.cmake](cmake/toolchains/arm-gcc.cmake). CPU model flags are supplied by
the selected platform.

The compiler search order is `aarch64-none-elf-gcc`, then
`aarch64-linux-gnu-gcc`, for AArch64. The 32-bit paths use `arm-none-eabi-gcc`.
Matching `objcopy`, `objdump`, `size`, and `readelf` tools are required.
Override the compiler prefix, including the final `-`, with `CROSS_COMPILE`:

```sh
make ARCH=aarch64 BUILD_DIR=build/custom-tools \
    CMAKE_ARGS='-DCROSS_COMPILE=/opt/aarch64-none-elf/bin/aarch64-none-elf- -DBOOT_GDB=/usr/bin/gdb-multiarch'
```

## Boot flow

The normal build creates two independently linked images:

```text
Reset -> bootloader1 -> bootloader2 -> optional U-Boot or kernel
```

Bootloader1 always transfers to `BOOTLOADER2_ENTRY_ADDRESS`, which each platform
defines with the bootloader2 memory regions. Bootloader2 transfers to the payload
when `CONFIG_BOOTLOADER2_AUTO_BOOT=y`. AArch64 and AArch32 pass four configured
argument registers and branch directly. Cortex-M treats the entry address as a
vector-table base, loads MSP and the reset handler, updates VTOR, and branches to
the reset handler.

The handoff code does not read a filesystem or storage device. The platform must
make the next image available at its linked or configured address before the
jump. QEMU loads both stage images automatically; secure flash platforms use the
combined `firmware.bin` image.

For QEMU, `BOOT_PAYLOAD_IMAGE` names a payload file. The default `raw` format
loads it at `CONFIG_PAYLOAD_LOAD_ADDRESS`; `BOOT_PAYLOAD_FORMAT=elf` uses the
ELF load addresses. Bootloader2 branches to `CONFIG_PAYLOAD_ENTRY_ADDRESS` in
both cases. Save these values as a fragment such as
`platform/qemu/virt-secure/configs/payload.config`:

```text
CONFIG_BOOTLOADER2_AUTO_BOOT=y
CONFIG_PAYLOAD_LOAD_ADDRESS=0x42000000
CONFIG_PAYLOAD_ENTRY_ADDRESS=0x42000000
CONFIG_PAYLOAD_ARGUMENT_0=0x40000000
CONFIG_PAYLOAD_ARGUMENT_1=0x0
CONFIG_PAYLOAD_ARGUMENT_2=0x0
CONFIG_PAYLOAD_ARGUMENT_3=0x0
```

Build and run with the fragment and raw image:

```sh
make run ARCH=aarch64 PLATFORM=qemu/virt-secure \
    CONFIG='aarch64_defconfig,payload.config' \
    CMAKE_ARGS='-DBOOT_PAYLOAD_IMAGE=/absolute/path/to/Image'
```

For an ELF payload, add `-DBOOT_PAYLOAD_FORMAT=elf` to `CMAKE_ARGS`.

The payload must match the current architecture, execution level or processor
mode, endianness, and calling convention. For AArch64 Linux,
`CONFIG_PAYLOAD_ARGUMENT_0` normally carries the DTB address. AArch32 payloads
can use all four argument settings for their entry protocol. A Cortex-M payload
uses its vector table and ignores the argument settings.

## Build artifacts and memory-map viewer

A normal build or the `boot` target produces the following files in `BUILD_DIR`:

| File | Contents |
| --- | --- |
| `bootloader1.elf`, `bootloader2.elf` | Linked stage images and debug symbols |
| `bootloader1.bin`, `bootloader2.bin` | Raw stage load images |
| `bootloader1.hex`, `bootloader2.hex` | Intel HEX records using load addresses |
| `bootloader1.asm`, `bootloader2.asm` | Disassembly with source lines |
| `bootloader1.map`, `bootloader2.map` | Linker section and symbol reports |
| `bootloader1.ld`, `bootloader2.ld` | Preprocessed stage linker scripts |
| `firmware.bin` | Address-preserving combined BL1 and BL2 image |
| `boot.gdb` | GDB connection script |
| `bootloader*-memory-map.svg` | Standalone memory-layout diagrams |
| `bootloader*-memory-map.html` | Browser viewers for both stages |
| `bootloader*-memory-map.json` | Stage region and section addresses |
| `compile_commands.json` | Compiler invocations for editor tooling |
| `clangd.config` | Editor configuration for this build directory |

Open the HTML file directly in a browser; no server or graphics package is needed:

```sh
xdg-open build/aarch64/bootloader1-memory-map.html
xdg-open build/aarch64/bootloader2-memory-map.html
```

Each diagram uses symbols from its stage ELF, including custom memory-header
values. It shows code, RO, RW, reserved gaps, and the stack with exact boundaries.
Region heights are schematic; usage bars compare linked bytes with capacity.
Stack allocation is shown separately from runtime stack usage, which is not
measured. Reserved ranges do not imply installed RAM.

Artifact generation runs on every normal build and every `boot` target invocation,
even if the ELF files need no relink. Deleted exports are regenerated. The
`bootloader1_image` and `bootloader2_image` targets link only their ELF files;
individual library targets build only the selected component.

### clangd

`CMAKE_EXPORT_COMPILE_COMMANDS` defaults to `ON`. Configure and build update the
root `.clangd` to reference the selected build directory's compilation database.
The generated configuration removes the GCC-only libc loop-optimization flag
from editor parsing; actual compiler commands retain it.

The latest configure or image build selects the active editor configuration.
When switching architectures, configure the desired directory again:

```sh
make configure ARCH=aarch64
```

`.clangd` is generated and ignored by Git. Its per-build copy is `clangd.config`.
Reload clangd if the editor does not pick up the updated configuration.

## Debugging

Start the platform debug server in one terminal, then connect in another:

```sh
# Terminal 1
make debug ARCH=aarch64

# Terminal 2
make gdb ARCH=aarch64
```

GDB connects to `127.0.0.1:1234`, loads symbols for both stages, and sets a
temporary breakpoint at `boot_main`. Image loading and target reset are
responsibilities of the platform backend.

```gdb
# Step startup, then continue to boot_main.
si
continue

# Inspect initialized data, BSS, and the stack.
p/x boot_data_cookie
p/x boot_bss_cookie
info registers
p/x &__stack_top

# Stop after startup checks.
break boot_ready
break default_exception_handler
continue
p boot_status
```

At `boot_ready`, the data cookie should match `BOOT_DATA_COOKIE_INITIAL`, BSS
should be zero, and status should match `BOOT_STATUS_READY`. Definitions are in
[boot_status.h](include/boot_status.h). Use **Ctrl+C** to interrupt execution in
GDB; finish with `detach` and `quit`.

Select a different port for simultaneous sessions in a board configuration:

```text
CONFIG_GDB_PORT=2345
```

CMake searches for the compiler-prefix GDB, then `gdb-multiarch`. Set `BOOT_GDB`
to choose it explicitly. Availability of `run` and `debug` depends on the platform.

## Memory layout

The platform's `include/platform_memory.h` defines all region boundaries.
[include/memory_layout.h](include/memory_layout.h) connects the platform header,
and [boot/boot.ld.S](boot/boot.ld.S) places sections and validates the layout.

```text
High address
+------------------------+  STACK_START = __stack_top (initial SP)
| Stack (grows downward)  |
+------------------------+  STACK_END = __stack_bottom
| Reserved               |
+------------------------+  RW_DATA_END
| RW: .data + .bss        |
+------------------------+  RW_DATA_START
| Reserved               |
+------------------------+  RO_DATA_END
| RO: .rodata + data init |
+------------------------+  CODE_END = RO_DATA_START
| Code + vector table    |
+------------------------+  CODE_START
Low address
```

All upper boundaries are exclusive. **`STACK_START` is the initial SP at the
high address; `STACK_END` is the stack's low boundary.** Region macros describe
allocated capacity; the generated report also shows actual linked contents.

`.data` initializers have a load address (LMA) in RO and a runtime address (VMA)
in RW. Startup copies these bytes and clears `.bss` and the stack. Reserved gaps are excluded
from initialization. `.bss` and `.stack` use `NOLOAD`, contributing no initial
bytes to either stage binary. RO/RW labels express linker placement, not MMU/MPU
protection.

### Custom memory headers

Edit the platform header or set `BOOT_MEMORY_CONFIG` to override selected defaults.
The custom header can replace the named stage boundaries while keeping the
bootloader2 entry synchronized with its code start:

```c
#ifndef MY_MEMORY_H
#define MY_MEMORY_H

#define BOOTLOADER2_CODE_START    0x00200000
#define BOOTLOADER2_RW_START      0x0e080000
#define BOOTLOADER2_STACK_END     0x0e0a0000
#define BOOTLOADER2_STACK_START   0x0e0b0000
#define BOOTLOADER2_ENTRY_ADDRESS BOOTLOADER2_CODE_START

#endif
```

Save it as `config/my_memory.h`, then build in a separate directory:

```sh
make ARCH=aarch64 BUILD_DIR=build/custom-memory \
    CMAKE_ARGS='-DBOOT_MEMORY_CONFIG=config/my_memory.h'
```

The resulting regions must fit the selected platform. Paths may be absolute or
relative to the repository root. Headers shared by C and GNU ld use integer
constants and arithmetic, without `U`/`UL` suffixes, casts, or `sizeof`.

The linker requires positive region sizes, 16-byte boundary alignment,
`CODE_END == RO_DATA_START`, `RO_DATA_END <= RW_DATA_START`, and
`RW_DATA_END <= STACK_END < STACK_START`. Actual contents must fit their regions.
Bootloader2 requires `CODE_START == BOOTLOADER2_ENTRY_ADDRESS`. Architecture
vector alignment and platform address limits are also validated.

## Implementation structure

| Location | Responsibility |
| --- | --- |
| `arch/` | Architecture startup, core exception handling, and assembly rules |
| `boot/` | Shared stage validation, linker script, image link, and artifacts |
| `boot/bl1/` | Bootloader1 entry and bootloader2 handoff |
| `boot/bl2/` | Bootloader2 entry and payload handoff |
| `include/arch/` | Architectural register fields, ABI, and core vector constants |
| `include/asm/` | Function, constant-load, address, and vector helper macros |
| `include/library/libc/` | Project-owned size, integer, memory, and string declarations |
| `driver/console/` | Generic console API and reusable UART IP drivers |
| `library/libc/` | Freestanding memory and string routines |
| `library/runtime/` | Stackless assembly for `.data` copy and `.bss`/stack clearing |
| `platform/` | Default selection, CPU models, devices, IRQs, and execution backends |
| `cmake/` | Generic configuration, toolchains, editor setup, and artifact exporter |

Each build directory owns its `CMakeLists.txt` and `KConfig`. `boot_arch` is an
OBJECT library. All selected driver and platform-binding sources are collected
in `libdriver.a`; libc and runtime sources are collected in `liblibrary.a`.
`boot_options` shares headers, definitions, and compiler flags.

AArch64 accepts EL1, EL2, and EL3 reset entry. EL2 descends to EL1h; EL3 remains
in EL3h. AArch32 selects SVC from HYP/SVC entry in RAM-boot builds. Secure-entry builds
select Monitor mode, the ARMv7-A security monitor. ARMv7-M loads MSP and PC from
reset vectors and configures VTOR. CPU selection and external IRQ configuration
come from the platform.

Address-zero secure firmware boot is the default for AArch64 and AArch32, using
EL3h and Monitor mode respectively. Platform selection is described in
the [Secure reset guide](platform/README.md#secure-reset-at-address-zero). The
reset entry is placed at the image base; exception vectors retain their required
alignment.

The assembly-only `runtime_init()` calls `boot_copy_data`, `boot_clear_bss`, and
`boot_clear_stack`, using registers rather than stack frames. Startup then sets SP
to `__stack_top` and calls `boot_main()`. No C code runs before initialization.
Default exception handlers wait without using a stack, preserving fault state
for GDB. The console API offers initialization, character/string output, and
hexadecimal formatting. `console_puts` converts LF to CRLF; `console_putc` emits
the byte unchanged. The libc subset provides `memcpy`, `memmove`, `memset`,
`memcmp`, and `strlen`; use `memmove` for overlapping buffers.

### Adding a platform

Create `platform/<vendor>/<board>/` with `platform.cmake`, `CMakeLists.txt`,
`KConfig`, architecture-specific files under `configs/`, a console binding, and
platform headers. Add the vendor and board symbols to their parent `KConfig`
files, then select them with `add_subdirectory_if_enabled` in the corresponding
parent `CMakeLists.txt`.

Define `BOOT_SUPPORTED_ARCHS`, `BOOT_CPU_FLAGS`, and `BOOT_CONSOLE_DRIVER`.
Optionally supply `BOOT_PLATFORM_TARGETS`, a CMake file defining `run` and `debug`.
The common build does not require an execution backend. Device addresses, clocks,
and external IRQ configuration belong in `platform_def.h`; region boundaries
and allowed ranges belong in `platform_memory.h`. See the
[Platform contract](platform/README.md#platform-contract) for included examples.

Shared runtime validation is implemented in [common.c](boot/common.c).
[bootloader1](boot/bl1/main.c) and [bootloader2](boot/bl2/main.c) own their stage
entry and handoff policy. Architecture-specific `boot_jump()` implementations
perform the final register and vector-table handoff. Interrupt support
requires controller/peripheral initialization, unmasking, and appropriate context
save/restore. Default handlers provide fault inspection rather than a scheduler.
### Assembly macros

Include [asm_macros.S](include/asm/asm_macros.S) to select the architecture's
helpers. CPU state, ABI alignment, and vector constants are defined in
[arch_def.h](include/arch/arch_def.h).

| Macro | Purpose |
| --- | --- |
| `func name, align=ARCH_FUNCTION_ALIGN_SHIFT, linkage=global, section=` | Function section, alignment, symbol type, linkage, and CFI |
| `endfunc name` | Close CFI and record function size |
| `unwind_root` | Stop debugger unwinding at reset/exception entry |
| `func_alias alias, target` | Global alias with Thumb attributes preserved |
| `weak_func_alias alias, target` | Alias replaceable by a strong definition |
| `mov_imm reg, value` | Load an integer constant without a literal pool |
| `load_addr reg, symbol` | Load a symbol address through linker relocations |
| `ldcopr` / `stcopr` | Access a named AArch32 CP15 register tuple |
| `vector_base` / `vector_entry` / `end_vector_entry` / `end_vector_table` | Validate AArch64 table alignment, slot size, and count |
| `vector_handler name` | Declare a Cortex-M vector and default weak handler |

`align` is log2(bytes); `linkage` is `global`, `local`, or `weak`. Reset functions
select `section=.text.boot`. Cortex-M functions automatically receive `.thumb_func`.
For example, an AArch64 leaf function:

```asm
#include "asm/asm_macros.S"
#define EXAMPLE_RESULT 0x1234000056780000

func read_example
    mov_imm x0, EXAMPLE_RESULT
    ret
endfunc read_example
```

AArch32/Cortex-M use `mov_imm r0, CONSTANT` and `bx lr`. Constants are assembled
from halfwords: `movw`/`movt` on 32-bit targets, `movz`/`movk` on AArch64.
32-bit destinations accept `-0x80000000` through `0xffffffff`; negative values
use two's complement. X registers use GNU assembler's 64-bit integer expressions.
Use assembly constants without C integer suffixes.

Load linker symbols with `load_addr x0, __stack_top` or
`load_addr r0, __stack_top`. AArch64 uses `adrp`/`add` within approximately +/-4 GiB
of the PC; 32-bit targets use `movw`/`movt` relocations.

Functions must preserve registers and manage their own stacks according to the
ABI. Update CFI when changing SP or the frame pointer. CFI resides in ELF
`.debug_frame`; reset and default exception handlers terminate unwinding with
`unwind_root`.

AArch64 vector slots have named symbols such as `aarch64_vector_current_spx_sync`
for GDB breakpoints. Each slot is 128 bytes and the table is 2048 bytes; oversized
definitions fail assembly. AArch32 validates eight slots. Cortex-M validates
the core vectors plus the external IRQ count supplied by the platform.

## Source conventions

Sources use copyright/SPDX headers, `File` and `Brief` descriptions, and
responsibility-based section comments. Platform names, machine assumptions,
and backend dependencies belong under `platform/`, including documentation.
See [Source file structure](docs/source-style.md) for templates and section order.

## License

Project-owned files use the [MIT License](LICENSE). The assembly helpers listed
in [Third-party notices](THIRD_PARTY_NOTICES.md) retain BSD-3-Clause licensing.
Reference trees retain their own licenses and are not build dependencies.
