# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/cmsdk_apb_uart/CMakeLists.txt
# Brief: Build the cmsdk_apb_uart console backend.

# Library target -------------------------------------------------------------
add_library(boot_console_driver STATIC "${CMAKE_CURRENT_LIST_DIR}/cmsdk_apb_uart.c")
target_link_libraries(boot_console_driver PUBLIC boot_options)
