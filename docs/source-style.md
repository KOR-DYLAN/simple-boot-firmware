# Source file structure

Use English for file headers, section labels, and implementation comments.
Describe the file's current responsibility and constraints, without change history.

## File header

Every CMake, C, header, assembly, and linker source starts with a copyright notice,
an SPDX license identifier, its repository-relative path, and a one-line purpose.
Project-owned files use MIT. Preserve the copyright and BSD-3-Clause identifiers
in files that already carry those notices.

C, `.h`, preprocessed `.S`, and linker sources use block comments:

```c
/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/example.h
 * Brief: Declare the example interface.
 */
```

CMake files use line comments:

```cmake
# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: example/CMakeLists.txt
# Brief: Build the example component.
```

Use `/* ... */` for `.S` comments. A leading `#` can be interpreted as a C
preprocessor directive. The linker source is `boot/boot.ld.S`;
CMake generates `boot.ld` from it. Edit the source, not the generated file.

## Section order

Use only sections that contain code. A small target definition needs one target
section; it does not need empty include, variable, or function sections.

| File type | Section order |
| --- | --- |
| `.c` | Includes, macro definitions, types, state, private helpers, public implementation |
| `.h` | Include guard, includes, macro definitions, types, extern objects, public API |
| `.S` startup | Includes, assembler configuration, reset/vector/handler sections in placement order |
| `.S` helpers | Include guard, includes, macro groups by function |
| `.ld` / `.ld.S` | Includes if preprocessed, entry point, memory regions, program headers, output sections, assertions |
| CMake | Requirements/configuration, validation, dependencies, targets, target properties and commands |

Section labels name a responsibility, such as `Platform compatibility checks` or
`Driver operations`. Use a single dashed comment line and a blank line before it:

```c
/* Includes --------------------------------------------------------------- */
#include <stdint.h>

/* Macro definitions ------------------------------------------------------ */
#define EXAMPLE_READY 1

/* Public API ------------------------------------------------------------- */
void example_init(void);
```

```cmake
# Library target -------------------------------------------------------------
add_library(example STATIC example.c)
target_link_libraries(example PUBLIC boot_options)
```

## CMake conventions

- Parent directories select child directories; leaf directories own their sources.
- Keep target creation beside its include paths, definitions, and dependencies.
- Separate configuration, compatibility checks, and generated-file commands in
  larger files. Keep short component files compact.
- Group related compiler flags together. Put long properties and command arguments
  on separate lines, with the closing parenthesis on its own line.
- Explain non-obvious requirements, such as using the same preprocessing inputs
  for C and the linker script. Avoid comments that merely repeat each command.

## Implementation comments

Document constraints and reasons: reset state assumptions, register requirements,
integer width limits, initialization ordering, and linker placement rules.
Keep comments next to the code they explain. Use named macros for hardware values
and layout constants. Do not add unused declarations or placeholder sections to
match the template.

## Platform boundary

CPU model names, device addresses, external IRQ counts, emulator settings, and
machine-specific documentation belong under `platform/`. Common architecture
code retains architectural rules and consumes platform-provided definitions.
Build reports and editor configuration are generated from the selected build.

## Alignment

Indent C and assembly bodies with four spaces. Align instruction operands after
an eight-character mnemonic field; longer macro names use one separating space.
Align macro values within contiguous definition groups. Keep labels and
preprocessor directives at their existing structural level.
