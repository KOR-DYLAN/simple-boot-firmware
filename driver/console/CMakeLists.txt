# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/console/CMakeLists.txt
# Brief: Select the UART backend and build the common console API.

# Console backend validation -------------------------------------------------
if(NOT BOOT_CONSOLE_DRIVER MATCHES "^[a-z0-9_]+$" OR
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BOOT_CONSOLE_DRIVER}/CMakeLists.txt")
    message(FATAL_ERROR "Unknown console driver: ${BOOT_CONSOLE_DRIVER}")
endif()

# Console targets ------------------------------------------------------------
add_subdirectory("${BOOT_CONSOLE_DRIVER}")
add_library(boot_console STATIC "${CMAKE_CURRENT_LIST_DIR}/console.c")
target_link_libraries(boot_console PUBLIC boot_options boot_platform)
