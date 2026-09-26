# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: arch/aarch32/CMakeLists.txt
# Brief: Build the aarch32 startup object library.

# Library target -------------------------------------------------------------
add_library_if_enabled(
    CONFIG_ARCH_AARCH32 boot_arch OBJECT
    startup.S handoff.S panic.c
)
target_link_libraries_if_enabled(CONFIG_ARCH_AARCH32 boot_arch PRIVATE boot_options)
