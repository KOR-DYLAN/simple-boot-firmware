# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/pl011/CMakeLists.txt
# Brief: Build the pl011 console backend.

# Library target -------------------------------------------------------------
add_library(boot_console_driver STATIC "${CMAKE_CURRENT_LIST_DIR}/pl011.c")
target_link_libraries(boot_console_driver PUBLIC boot_options)
