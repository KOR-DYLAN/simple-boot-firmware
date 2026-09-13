# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/toolchains/aarch64.cmake
# Brief: Select the aarch64 GNU cross-toolchain configuration.

# Architecture and common toolchain ------------------------------------------
set(BOOT_TOOLCHAIN_ARCH aarch64)
include("${CMAKE_CURRENT_LIST_DIR}/arm-gcc.cmake")
