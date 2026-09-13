# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/toolchains/arm-gcc.cmake
# Brief: Configure GNU cross tools and bare-metal compiler discovery.

# Architecture compatibility -------------------------------------------------
if(DEFINED BOOT_TOOLCHAIN_ARCH)
    if(DEFINED BOOT_ARCH AND NOT BOOT_ARCH STREQUAL BOOT_TOOLCHAIN_ARCH)
        message(FATAL_ERROR
            "Toolchain ${BOOT_TOOLCHAIN_ARCH} does not support BOOT_ARCH=${BOOT_ARCH}"
        )
    endif()
    set(BOOT_ARCH "${BOOT_TOOLCHAIN_ARCH}" CACHE STRING "Target architecture")
endif()

# Cross-compilation environment ----------------------------------------------
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR "${BOOT_ARCH}")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES BOOT_ARCH CROSS_COMPILE)

# Compiler prefix selection --------------------------------------------------
if(NOT CROSS_COMPILE)
    if(BOOT_ARCH STREQUAL "aarch64")
        find_program(_boot_gcc NAMES aarch64-none-elf-gcc aarch64-linux-gnu-gcc REQUIRED)
    else()
        find_program(_boot_gcc NAMES arm-none-eabi-gcc REQUIRED)
    endif()
    string(REGEX REPLACE "gcc$" "" _boot_prefix "${_boot_gcc}")
    set(CROSS_COMPILE "${_boot_prefix}"
        CACHE STRING "Cross compiler prefix, including optional absolute path"
    )
endif()

# GNU tool discovery ---------------------------------------------------------
find_program(CMAKE_C_COMPILER NAMES "${CROSS_COMPILE}gcc" REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES "${CROSS_COMPILE}gcc" REQUIRED)
find_program(CMAKE_OBJCOPY NAMES "${CROSS_COMPILE}objcopy" REQUIRED)
find_program(CMAKE_OBJDUMP NAMES "${CROSS_COMPILE}objdump" REQUIRED)
find_program(CMAKE_SIZE NAMES "${CROSS_COMPILE}size" REQUIRED)
find_program(CMAKE_READELF NAMES "${CROSS_COMPILE}readelf" REQUIRED)

# Host and target search rules -----------------------------------------------
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
