# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/mps2-an385/platform.cmake
# Brief: Define CPU, console, and QEMU settings for mps2-an385.

# Platform capabilities ------------------------------------------------------
set(BOOT_SUPPORTED_ARCHS cortex-m)
set(BOOT_CONSOLE_DRIVER cmsdk_apb_uart)
set(BOOT_PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../handoff.c")
set(BOOT_PLATFORM_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include")
set(BOOT_PLATFORM_IMAGE1_DEFINES PLATFORM_IMAGE=1)
set(BOOT_PLATFORM_IMAGE2_DEFINES PLATFORM_IMAGE=2)

# CPU compilation options ----------------------------------------------------
set(BOOT_CPU_FLAGS -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-unaligned-access)

# QEMU machine configuration -------------------------------------------------
set(BOOT_QEMU_NAME qemu-system-arm)
set(BOOT_QEMU_ARGS -machine mps2-an385 -cpu cortex-m3)
set(BOOT_QEMU_LOAD_MODE kernel)

# Execution backend ----------------------------------------------------------
set(BOOT_PLATFORM_TARGETS "${CMAKE_CURRENT_LIST_DIR}/../targets.cmake")
