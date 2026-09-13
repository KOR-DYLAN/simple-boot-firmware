# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/BootConfig.cmake
# Brief: Select and validate architecture, platform, and user settings.

# Architecture selection -----------------------------------------------------
set(BOOT_ARCH aarch64 CACHE STRING "Target: aarch64, aarch32, cortex-m")
set_property(CACHE BOOT_ARCH PROPERTY STRINGS aarch64 aarch32 cortex-m)
if(NOT BOOT_ARCH MATCHES "^(aarch64|aarch32|cortex-m)$")
    message(FATAL_ERROR "BOOT_ARCH must be aarch64, aarch32, or cortex-m")
endif()

# Toolchain and build configuration ------------------------------------------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${BOOT_ARCH}.cmake"
        CACHE FILEPATH "Cross toolchain"
    )
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Compilation database ------------------------------------------------------
if(NOT DEFINED CMAKE_EXPORT_COMPILE_COMMANDS OR CMAKE_EXPORT_COMPILE_COMMANDS STREQUAL "")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compiler commands" FORCE)
endif()

# Build-directory architecture check -----------------------------------------
if(DEFINED BOOT_CONFIGURED_ARCH AND NOT BOOT_CONFIGURED_ARCH STREQUAL BOOT_ARCH)
    message(FATAL_ERROR "Use a separate build directory for each BOOT_ARCH")
endif()
set(BOOT_CONFIGURED_ARCH "${BOOT_ARCH}" CACHE INTERNAL "Architecture of this build directory")

# Memory and debugger options ------------------------------------------------
set(BOOT_MEMORY_CONFIG "" CACHE FILEPATH "Optional header overriding memory boundaries")
set(BOOT_GDB_PORT 1234 CACHE STRING "Debug server port")
if(NOT BOOT_GDB_PORT MATCHES "^[0-9]+$" OR BOOT_GDB_PORT LESS 1 OR BOOT_GDB_PORT GREATER 65535)
    message(FATAL_ERROR "BOOT_GDB_PORT must be between 1 and 65535")
endif()

# Platform selection ---------------------------------------------------------
set(BOOT_PLATFORM "" CACHE STRING "Platform directory under platform/")
include("${CMAKE_CURRENT_SOURCE_DIR}/platform/defaults.cmake")
if(NOT BOOT_PLATFORM MATCHES "^[a-z0-9_-]+(/[a-z0-9_-]+)+$")
    message(FATAL_ERROR "Invalid BOOT_PLATFORM directory: ${BOOT_PLATFORM}")
endif()
set(BOOT_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platform/${BOOT_PLATFORM}")
if(NOT EXISTS "${BOOT_PLATFORM_DIR}/platform.cmake")
    message(FATAL_ERROR "Platform not found: ${BOOT_PLATFORM_DIR}/platform.cmake")
endif()
include("${BOOT_PLATFORM_DIR}/platform.cmake")

# Platform compatibility checks ----------------------------------------------
if(NOT BOOT_ARCH IN_LIST BOOT_SUPPORTED_ARCHS)
    message(FATAL_ERROR
        "Platform ${BOOT_PLATFORM} does not support BOOT_ARCH=${BOOT_ARCH}; "
        "supported: ${BOOT_SUPPORTED_ARCHS}"
    )
endif()
if(DEFINED BOOT_CONFIGURED_PLATFORM AND NOT BOOT_CONFIGURED_PLATFORM STREQUAL BOOT_PLATFORM)
    message(FATAL_ERROR "Use a separate build directory for each BOOT_PLATFORM")
endif()
set(BOOT_CONFIGURED_PLATFORM "${BOOT_PLATFORM}" CACHE INTERNAL "Platform of this build directory")
message(STATUS "Boot platform: ${BOOT_PLATFORM} (${BOOT_ARCH}), console: ${BOOT_CONSOLE_DRIVER}")

# Memory override validation -------------------------------------------------
if(BOOT_MEMORY_CONFIG)
    get_filename_component(BOOT_MEMORY_CONFIG "${BOOT_MEMORY_CONFIG}"
        ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if(NOT EXISTS "${BOOT_MEMORY_CONFIG}")
        message(FATAL_ERROR "BOOT_MEMORY_CONFIG does not exist: ${BOOT_MEMORY_CONFIG}")
    endif()
endif()
