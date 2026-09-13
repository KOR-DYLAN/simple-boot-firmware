# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/defaults.cmake
# Brief: Define available platforms and architecture-specific defaults.

# Platform defaults ----------------------------------------------------------
set_property(CACHE BOOT_PLATFORM PROPERTY STRINGS qemu/virt qemu/virt-secure qemu/mps2-an385)
if(NOT BOOT_PLATFORM)
    if(BOOT_ARCH STREQUAL "cortex-m")
        set(BOOT_PLATFORM qemu/mps2-an385 CACHE STRING "Platform directory" FORCE)
    else()
        set(BOOT_PLATFORM qemu/virt-secure CACHE STRING "Platform directory" FORCE)
    endif()
endif()
