# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: arch/cortex-m/CMakeLists.txt
# Brief: Build the cortex-m startup object library.

# Library target -------------------------------------------------------------
add_library(boot_arch OBJECT startup.S)
target_link_libraries(boot_arch PRIVATE boot_options)
