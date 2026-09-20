# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: boot/bl2/CMakeLists.txt
# Brief: Build the bootloader2 image.

# Bootloader stage -----------------------------------------------------------
add_boot_stage(${BOOT_STAGE_2} bootloader2 main.c)
