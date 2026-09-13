# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: library/runtime/CMakeLists.txt
# Brief: Build data-copy and BSS-initialization routines.

# Architecture-specific assembly runtime -------------------------------------
if(BOOT_ARCH STREQUAL "aarch64")
    set(boot_runtime_source aarch64.S)
else()
    set(boot_runtime_source aarch32.S)
endif()

# Library target -------------------------------------------------------------
add_library(boot_runtime STATIC "${boot_runtime_source}")
# The C archive rule recreates the archive from the selected object list.
set_target_properties(boot_runtime PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(boot_runtime PUBLIC boot_options)
