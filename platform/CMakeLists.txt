# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/CMakeLists.txt
# Brief: Select the platform vendor and board path.

# Subdirectory selection -----------------------------------------------------
string(REPLACE "/" ";" boot_platform_parts "${BOOT_PLATFORM}")
list(POP_FRONT boot_platform_parts boot_platform_vendor)
list(JOIN boot_platform_parts "/" BOOT_PLATFORM_BOARD)
add_subdirectory("${boot_platform_vendor}")
