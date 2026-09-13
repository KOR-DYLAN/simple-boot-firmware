# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/toolchains/cortex-m.cmake
# Brief: Select the cortex-m GNU cross-toolchain configuration.

# Architecture and common toolchain ------------------------------------------
set(BOOT_TOOLCHAIN_ARCH cortex-m)
include("${CMAKE_CURRENT_LIST_DIR}/arm-gcc.cmake")
