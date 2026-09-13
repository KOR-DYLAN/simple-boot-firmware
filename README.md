# simple-boot

Minimal bare-metal startup for AArch64, AArch32, and ARMv7-M. Build with CMake,
inspect the generated memory map, and debug with GDB. Platform code owns CPU
model selection, device addresses, external interrupts, and execution backends.
See the [Platform guide](platform/README.md) for supported configurations.

## Quick start

Install GNU Make, CMake 3.21 or later, Ninja, Python 3.8 or later, and a GNU cross
toolchain. Add a GDB that supports the target architecture and the execution
tools described by the selected platform.

```sh
sudo apt install make cmake ninja-build python3 \
    gcc-aarch64-linux-gnu gcc-arm-none-eabi gdb-multiarch

make ARCH=aarch64
make run ARCH=aarch64
```

Use `ARCH=aarch32` or `ARCH=cortex-m` for the other architecture paths. The
platform directory defines defaults when `PLATFORM` is omitted. Startup prints
memory boundaries and `BOOT OK [architecture]`, then waits. Initialization
failures print `BOOT FAIL` before halting.

The implementation uses freestanding C11 and GNU assembly. Startup assumes
little-endian execution with the MMU and caches disabled. Interrupts remain
masked. The included console drivers use polling. The project provides a small
memory/string library, with no OS, heap, C++ runtime, or floating-point/SIMD setup.

## Build commands

| Command | Action |
| --- | --- |
| `make` or `make build` | Configure, compile, and generate all artifacts |
| `make configure` | Configure and select the editor compilation database |
| `make run` | Build and invoke the platform execution target |
| `make debug` | Build and invoke the platform debug-server target |
| `make gdb` | Build and connect GDB to the debug server |
| `make layout` | Generate artifacts and print ELF sections/program headers |
| `make target TARGET=boot_libc` | Build a specific component |
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
| `PLATFORM` | Platform-owned default | Directory under `platform/` |
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

## Build artifacts and memory-map viewer

A normal build or the `boot` target produces the following files in `BUILD_DIR`:

| File | Contents |
| --- | --- |
| `boot.elf` | Linked image, entry point, and debug symbols |
| `boot.bin` | Raw load image beginning at `CODE_START` |
| `boot.hex` | Intel HEX records using load addresses |
| `boot.asm` | Disassembly with available source lines |
| `boot.map` | Linker section and symbol report |
| `boot.ld` | Preprocessed linker script |
| `boot.gdb` | GDB connection script |
| `memory-map.svg` | Standalone memory-layout diagram |
| `memory-map.html` | Browser viewer with linked-content addresses and sizes |
| `memory-map.json` | Region boundaries, capacity, usage, and section addresses |
| `compile_commands.json` | Compiler invocations for editor tooling |
| `clangd.config` | Editor configuration for this build directory |

Open the HTML file directly in a browser; no server or graphics package is needed:

```sh
xdg-open build/aarch64/memory-map.html
```

The diagram uses symbols from the final ELF, including custom memory-header
values. It shows code, RO, RW, reserved gaps, and the stack with exact boundaries.
Region heights are schematic; usage bars compare linked bytes with capacity.
Stack allocation is shown separately from runtime stack usage, which is not
measured. Reserved ranges do not imply installed RAM.

Artifact generation runs on every normal build and every `boot` target invocation,
even if the ELF needs no relink. Deleted exports are regenerated. The internal
`boot_image` target links only the ELF; individual library targets build only
the selected component.

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

GDB connects to `127.0.0.1:1234` by default and sets a temporary breakpoint at
`boot_main`. Image loading and target reset are responsibilities of the platform
backend; consult its guide before issuing additional GDB commands.

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

Select a different port for simultaneous sessions:

```sh
make debug ARCH=aarch64 CMAKE_ARGS='-DBOOT_GDB_PORT=2345'
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
bytes to `boot.bin`. RO/RW labels express linker placement, not MMU/MPU protection.

### Custom memory headers

Edit the platform header or set `BOOT_MEMORY_CONFIG` to override selected defaults.
For example, a header can place RW relative to the selected RO end:

```c
#ifndef MY_MEMORY_H
#define MY_MEMORY_H

#define CUSTOM_RESERVED_GAP 0x00100000
#define RW_DATA_START (RO_DATA_END + CUSTOM_RESERVED_GAP)

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
Architecture vector alignment and platform address limits are also validated.

## Implementation structure

| Location | Responsibility |
| --- | --- |
| `arch/` | Architecture startup, core exception handling, and assembly rules |
| `boot/` | Application entry, linker script, ELF link, and artifact target |
| `include/arch/` | Architectural register fields, ABI, and core vector constants |
| `include/asm/` | Function, constant-load, address, and vector helper macros |
| `driver/console/` | Generic console API and reusable UART IP drivers |
| `library/libc/` | Freestanding memory and string routines |
| `library/runtime/` | Stackless assembly for `.data` copy and `.bss`/stack clearing |
| `platform/` | Default selection, CPU models, devices, IRQs, and execution backends |
| `cmake/` | Generic configuration, toolchains, editor setup, and artifact exporter |

Each source directory owns its `CMakeLists.txt`. `boot_arch` is an OBJECT
library; libc, runtime, console, driver, and platform are static libraries.
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

Create `platform/<vendor>/<board>/` with a `platform.cmake`, `CMakeLists.txt`,
console binding, and platform headers. The vendor CMake file selects the board
with `add_subdirectory("${BOOT_PLATFORM_BOARD}")`.

Define `BOOT_SUPPORTED_ARCHS`, `BOOT_CPU_FLAGS`, and `BOOT_CONSOLE_DRIVER`.
Optionally supply `BOOT_PLATFORM_TARGETS`, a CMake file defining `run` and `debug`.
The common build does not require an execution backend. Device addresses, clocks,
and external IRQ configuration belong in `platform_def.h`; region boundaries
and allowed ranges belong in `platform_memory.h`. See the
[Platform contract](platform/README.md#platform-contract) for included examples.

Application functionality starts in [boot/main.c](boot/main.c). Interrupt support
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
