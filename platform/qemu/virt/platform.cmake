# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/virt/platform.cmake
# Brief: Define CPU, console, and QEMU settings for virt.

# Platform capabilities ------------------------------------------------------
set(BOOT_SUPPORTED_ARCHS aarch64 aarch32)
set(BOOT_CONSOLE_DRIVER pl011)
set(BOOT_QEMU_LOAD_MODE loader)

# Architecture-specific CPU settings -----------------------------------------
if(BOOT_ARCH STREQUAL "aarch64")
    set(BOOT_CPU_FLAGS -mcpu=cortex-a53 -mgeneral-regs-only -mstrict-align)
    set(BOOT_QEMU_NAME qemu-system-aarch64)
    set(BOOT_QEMU_CPU cortex-a53)
else()
    set(BOOT_CPU_FLAGS -mcpu=cortex-a15 -marm -mfloat-abi=soft -mno-unaligned-access)
    set(BOOT_QEMU_NAME qemu-system-arm)
    set(BOOT_QEMU_CPU cortex-a15)
endif()

# QEMU machine configuration -------------------------------------------------
set(BOOT_QEMU_GIC_VERSION 2)
set(BOOT_QEMU_RAM_SIZE 128M)
set(BOOT_QEMU_CPU_COUNT 1)
set(BOOT_QEMU_ARGS -machine virt,virtualization=on,gic-version=${BOOT_QEMU_GIC_VERSION}
    -cpu ${BOOT_QEMU_CPU} -m ${BOOT_QEMU_RAM_SIZE} -smp ${BOOT_QEMU_CPU_COUNT})

# Execution backend ----------------------------------------------------------
set(BOOT_PLATFORM_TARGETS "${CMAKE_CURRENT_LIST_DIR}/../targets.cmake")
