# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: driver/CMakeLists.txt
# Brief: Build the aggregate driver library.

# Aggregate library ----------------------------------------------------------
add_library_if_enabled(CONFIG_DRIVER libdriver STATIC)
set_target_properties(libdriver PROPERTIES PREFIX "")
target_link_libraries_if_enabled(CONFIG_DRIVER libdriver PUBLIC boot_options)

# Driver composition ---------------------------------------------------------
add_subdirectory_if_enabled(CONFIG_CONSOLE console)
