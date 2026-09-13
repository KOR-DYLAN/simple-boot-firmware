# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/toolchains/aarch32.cmake
# Brief: Select the aarch32 GNU cross-toolchain configuration.

# Architecture and common toolchain ------------------------------------------
set(BOOT_TOOLCHAIN_ARCH aarch32)
include("${CMAKE_CURRENT_LIST_DIR}/arm-gcc.cmake")
