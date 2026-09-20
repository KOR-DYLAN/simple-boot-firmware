# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/pl011/CMakeLists.txt
# Brief: Add the PL011 console backend to the driver library.

# Driver sources -------------------------------------------------------------
add_source_if_enabled(CONFIG_DRIVER_PL011 pl011.c)
