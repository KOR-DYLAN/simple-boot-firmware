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
    "${BOOT_KCONFIG_OUTPUT_DIR}"
    "${BOOT_PLATFORM_DIR}/include"
    ${BOOT_PLATFORM_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/library/libc"
)

set(BOOT_DEFINES "${BOOT_ARCH_DEFINE}" ${BOOT_PLATFORM_DEFINES})
if(BOOT_MEMORY_CONFIG)
    list(APPEND BOOT_DEFINES "BOOT_MEMORY_CONFIG=\"${BOOT_MEMORY_CONFIG}\"")
endif()

# Shared compiler options target ---------------------------------------------
add_library(boot_options INTERFACE)
target_include_directories(boot_options INTERFACE ${BOOT_INCLUDE_DIRS})
target_compile_definitions(boot_options INTERFACE ${BOOT_DEFINES})
target_compile_features(boot_options INTERFACE c_std_99)
target_compile_options(boot_options INTERFACE
    ${BOOT_CPU_FLAGS}
    -include "${BOOT_AUTOCONF_HEADER}"
    "$<$<COMPILE_LANGUAGE:C>:-std=c99>"
    -g3
    -ffreestanding -fno-builtin -nostdinc
    -fno-stack-protector -fno-pic -fno-pie
    -fno-unwind-tables -fno-asynchronous-unwind-tables
    -ffunction-sections -fdata-sections
    -Wall -Wextra -Werror
    "$<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>"
    "$<$<COMPILE_LANGUAGE:ASM>:-D__ASSEMBLER__=1>"
)

# Configuration targets ------------------------------------------------------
set(BOOT_BASE_CONFIG_ARGS)
foreach(boot_config_file IN LISTS BOOT_CONFIG_FILES)
    list(APPEND BOOT_BASE_CONFIG_ARGS --config "${boot_config_file}")
endforeach()
set(BOOT_BASE_FRAGMENT_ARGS)
foreach(boot_config_file IN LISTS BOOT_CONFIG_FILES)
    list(APPEND BOOT_BASE_FRAGMENT_ARGS --base-config "${boot_config_file}")
endforeach()

add_custom_target(defconfig
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/kconfig.py"
            --source-dir "${PROJECT_SOURCE_DIR}"
            --kconfig "${PROJECT_SOURCE_DIR}/KConfig"
            ${BOOT_BASE_CONFIG_ARGS}
            --output-dir "${BOOT_KCONFIG_OUTPUT_DIR}"
            --clear-local "${BOOT_LOCAL_CONFIG_FILE}"
    COMMAND "${CMAKE_COMMAND}" -S "${PROJECT_SOURCE_DIR}" -B "${PROJECT_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(menuconfig
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/menuconfig.py"
            --source-dir "${PROJECT_SOURCE_DIR}"
            --kconfig "${PROJECT_SOURCE_DIR}/KConfig"
            ${BOOT_BASE_FRAGMENT_ARGS}
            --current-config "${PROJECT_BINARY_DIR}/.config"
            --local-config "${BOOT_LOCAL_CONFIG_FILE}"
            --output-dir "${BOOT_KCONFIG_OUTPUT_DIR}"
    COMMAND "${CMAKE_COMMAND}" -S "${PROJECT_SOURCE_DIR}" -B "${PROJECT_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(savedefconfig
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/kconfig.py"
            --source-dir "${PROJECT_SOURCE_DIR}"
            --kconfig "${PROJECT_SOURCE_DIR}/KConfig"
            --config "${PROJECT_BINARY_DIR}/.config"
            --output-dir "${BOOT_KCONFIG_OUTPUT_DIR}"
            --save-defconfig "${PROJECT_BINARY_DIR}/defconfig"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(savefragmentconfig
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/kconfig.py"
            --source-dir "${PROJECT_SOURCE_DIR}"
            --kconfig "${PROJECT_SOURCE_DIR}/KConfig"
            --config "${PROJECT_BINARY_DIR}/.config"
            ${BOOT_BASE_FRAGMENT_ARGS}
            --output-dir "${BOOT_KCONFIG_OUTPUT_DIR}"
            --save-fragment "${PROJECT_BINARY_DIR}/fragment.config"
    USES_TERMINAL
    VERBATIM
)
add_custom_target(fragmentconfig DEPENDS savefragmentconfig)
