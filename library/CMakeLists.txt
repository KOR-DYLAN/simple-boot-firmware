# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: library/CMakeLists.txt
# Brief: Build the aggregate freestanding library.

# Aggregate library ----------------------------------------------------------
add_library_if_enabled(CONFIG_LIBRARY liblibrary STATIC)
set_target_properties(liblibrary PROPERTIES PREFIX "" LINKER_LANGUAGE C)
target_link_libraries_if_enabled(CONFIG_LIBRARY liblibrary PUBLIC boot_options)

# Library composition --------------------------------------------------------
add_subdirectory_if_enabled(CONFIG_LIBC libc)
add_subdirectory_if_enabled(CONFIG_RUNTIME runtime)
