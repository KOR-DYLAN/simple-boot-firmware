# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/CMakeLists.txt
# Brief: Define shared include paths, macros, and compiler options.

# Build requirements ---------------------------------------------------------
get_property(BOOT_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(BOOT_MULTI_CONFIG)
    message(FATAL_ERROR
        "Use Ninja or Unix Makefiles with CMAKE_BUILD_TYPE=Debug or Release"
    )
endif()
if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This project uses the GNU Arm toolchain and GNU ld")
endif()

# Architecture macros --------------------------------------------------------
if(BOOT_ARCH STREQUAL "aarch64")
    set(BOOT_ARCH_DEFINE BOOT_ARCH_AARCH64=1)
elseif(BOOT_ARCH STREQUAL "aarch32")
    set(BOOT_ARCH_DEFINE BOOT_ARCH_AARCH32=1)
else()
    set(BOOT_ARCH_DEFINE BOOT_ARCH_CORTEX_M=1)
endif()

# Header paths and compile definitions ---------------------------------------
set(BOOT_INCLUDE_DIRS
    "${BOOT_PLATFORM_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/library/libc/include"
)

set(BOOT_DEFINES "${BOOT_ARCH_DEFINE}" ${BOOT_PLATFORM_DEFINES})
if(BOOT_MEMORY_CONFIG)
    list(APPEND BOOT_DEFINES "BOOT_MEMORY_CONFIG=\"${BOOT_MEMORY_CONFIG}\"")
endif()

# Shared compiler options target ---------------------------------------------
add_library(boot_options INTERFACE)
target_include_directories(boot_options INTERFACE ${BOOT_INCLUDE_DIRS})
target_compile_definitions(boot_options INTERFACE ${BOOT_DEFINES})
target_compile_features(boot_options INTERFACE c_std_11)
target_compile_options(boot_options INTERFACE
    ${BOOT_CPU_FLAGS}
    -g3
    -ffreestanding -fno-builtin
    -fno-stack-protector -fno-pic -fno-pie
    -fno-unwind-tables -fno-asynchronous-unwind-tables
    -ffunction-sections -fdata-sections
    -Wall -Wextra -Werror
    "$<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>"
)
