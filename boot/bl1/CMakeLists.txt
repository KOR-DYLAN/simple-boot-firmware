# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: boot/bl1/CMakeLists.txt
# Brief: Build the bootloader1 image.

# Bootloader stage -----------------------------------------------------------
add_boot_stage(${BOOT_STAGE_1} bootloader1 main.c)
