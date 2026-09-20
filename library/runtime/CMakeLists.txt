# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: library/runtime/CMakeLists.txt
# Brief: Add data-copy and zero-initialization routines to the library.

# Library sources ------------------------------------------------------------
add_source_if_enabled(CONFIG_RUNTIME_AARCH64 aarch64.S)
add_source_if_enabled(CONFIG_RUNTIME_AARCH32 aarch32.S)
