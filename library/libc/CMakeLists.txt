# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: library/libc/CMakeLists.txt
# Brief: Add the freestanding standard C subset to the library.

# Library sources ------------------------------------------------------------
add_source_if_enabled(CONFIG_LIBC ctype.c stdio.c stdlib.c string.c)

# Freestanding implementation options ----------------------------------------
# Keep GCC from replacing these loops with recursive libc calls.
set_source_files_properties(string.c TARGET_DIRECTORY liblibrary PROPERTIES
    COMPILE_OPTIONS -fno-tree-loop-distribute-patterns
)
