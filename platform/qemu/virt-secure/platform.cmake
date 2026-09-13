# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/virt-secure/platform.cmake
# Brief: Configure secure firmware reset from flash address zero.

# Shared CPU and console selection -------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/../virt/platform.cmake")

# Secure reset configuration -------------------------------------------------
set(BOOT_QEMU_LOAD_MODE bios)
set(BOOT_QEMU_ARGS
    -machine virt,secure=on,virtualization=on,gic-version=${BOOT_QEMU_GIC_VERSION}
    -cpu ${BOOT_QEMU_CPU}
    -m ${BOOT_QEMU_RAM_SIZE}
    -smp ${BOOT_QEMU_CPU_COUNT}
)
if(BOOT_ARCH STREQUAL "aarch32")
    set(BOOT_PLATFORM_DEFINES BOOT_AARCH32_SECURE_ENTRY=1)
endif()
