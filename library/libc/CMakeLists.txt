# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: library/libc/CMakeLists.txt
# Brief: Build freestanding memory and string routines.

# Library target -------------------------------------------------------------
add_library(boot_libc STATIC "${CMAKE_CURRENT_LIST_DIR}/string.c")
target_link_libraries(boot_libc PUBLIC boot_options)

# Freestanding implementation options ----------------------------------------
# Keep GCC from replacing these loops with recursive libc calls.
target_compile_options(boot_libc PRIVATE -fno-tree-loop-distribute-patterns)
