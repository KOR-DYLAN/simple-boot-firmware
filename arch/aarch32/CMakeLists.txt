# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: arch/aarch32/CMakeLists.txt
# Brief: Build the aarch32 startup object library.

# Library target -------------------------------------------------------------
add_library(boot_arch OBJECT startup.S)
target_link_libraries(boot_arch PRIVATE boot_options)
