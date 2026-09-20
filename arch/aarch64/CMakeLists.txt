# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: arch/aarch64/CMakeLists.txt
# Brief: Build the aarch64 startup object library.

# Library target -------------------------------------------------------------
add_library_if_enabled(
    CONFIG_ARCH_AARCH64 boot_arch OBJECT
    startup.S vectors.S handoff.S
)
target_link_libraries_if_enabled(CONFIG_ARCH_AARCH64 boot_arch PRIVATE boot_options)
