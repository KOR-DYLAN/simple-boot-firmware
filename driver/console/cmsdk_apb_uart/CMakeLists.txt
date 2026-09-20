# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/cmsdk_apb_uart/CMakeLists.txt
# Brief: Add the CMSDK APB UART backend to the driver library.

# Driver sources -------------------------------------------------------------
add_source_if_enabled(CONFIG_DRIVER_CMSDK_APB_UART cmsdk_apb_uart.c)
