# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: CMakeLists.txt
# Brief: Declare the project and compose the selected build components.

# Project setup --------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)

# Select the toolchain before project() enables the compilers.
include(cmake/BootConfig.cmake)
project(simple_boot VERSION 1.0 LANGUAGES C ASM)
include(cmake/EditorConfig.cmake)

# Build components -----------------------------------------------------------
add_subdirectory(cmake)
add_subdirectory(arch)
add_subdirectory(library)
add_subdirectory(driver)
add_subdirectory(platform)
add_subdirectory(boot)
