# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/CMakeLists.txt
# Brief: Select the UART backend and build the common console API.

# Console composition --------------------------------------------------------
add_source_if_enabled(CONFIG_CONSOLE console.c)
add_subdirectory_if_enabled(CONFIG_DRIVER_PL011 pl011)
add_subdirectory_if_enabled(CONFIG_DRIVER_CMSDK_APB_UART cmsdk_apb_uart)
