# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/mps2-an385/CMakeLists.txt
# Brief: Add the QEMU MPS2 AN385 console binding to the driver library.

# Driver sources -------------------------------------------------------------
add_source_if_enabled(CONFIG_PLATFORM_QEMU_MPS2_AN385 console.c)
