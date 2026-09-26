# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/virt-secure/CMakeLists.txt
# Brief: Add the shared QEMU PL011 console binding to the driver library.

# Driver sources -------------------------------------------------------------
add_source_if_enabled(CONFIG_PLATFORM_QEMU_VIRT_SECURE ../pl011_console.c)
