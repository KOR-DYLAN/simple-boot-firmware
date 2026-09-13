# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/mps2-an385/CMakeLists.txt
# Brief: Build the QEMU mps2-an385 platform console binding.

# Library target -------------------------------------------------------------
add_library(boot_platform STATIC "${CMAKE_CURRENT_LIST_DIR}/console.c")
target_link_libraries(boot_platform PUBLIC boot_options boot_console_driver)
