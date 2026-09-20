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

# Memory option --------------------------------------------------------------
set(BOOT_MEMORY_CONFIG "" CACHE FILEPATH "Optional header overriding memory boundaries")
set(BOOT_PAYLOAD_IMAGE "" CACHE FILEPATH "Optional U-Boot or kernel image")
set(BOOT_PAYLOAD_FORMAT raw CACHE STRING "Payload image format: raw or elf")
set_property(CACHE BOOT_PAYLOAD_FORMAT PROPERTY STRINGS raw elf)

# Board configuration selection ----------------------------------------------
set(BOOT_PLATFORM "" CACHE STRING "Platform directory under platform/")
set(BOOT_CONFIG "" CACHE STRING "Comma-separated board configuration file names")
if(BOOT_CONFIG MATCHES "[/\\]")
    if(DEFINED BOOT_CONFIGURED_PLATFORM)
        get_filename_component(BOOT_CONFIG "${BOOT_CONFIG}" NAME)
    else()
        message(FATAL_ERROR "BOOT_CONFIG entries must be file names without paths")
    endif()
endif()

if(BOOT_PLATFORM)
    if(NOT BOOT_PLATFORM MATCHES "^[a-z0-9_-]+(/[a-z0-9_-]+)+$")
        message(FATAL_ERROR "Invalid BOOT_PLATFORM directory: ${BOOT_PLATFORM}")
    endif()
    set(boot_config_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/${BOOT_PLATFORM}/configs"
    )
else()
    file(GLOB boot_default_configs CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/*/*/configs/${BOOT_ARCH}_defconfig"
    )
    list(LENGTH boot_default_configs boot_default_config_count)
    if(NOT boot_default_config_count EQUAL 1)
        message(FATAL_ERROR
            "Expected one platform/*/*/configs/${BOOT_ARCH}_defconfig; "
            "found ${boot_default_config_count}"
        )
    endif()
    list(GET boot_default_configs 0 boot_default_config)
    get_filename_component(boot_config_directory
        "${boot_default_config}" DIRECTORY
    )
endif()

if(BOOT_CONFIG)
    string(REPLACE "," ";" boot_config_names "${BOOT_CONFIG}")
    string(REGEX REPLACE "[ \t]+" ";" boot_config_names "${boot_config_names}")
    list(FILTER boot_config_names EXCLUDE REGEX "^$")
else()
    set(boot_config_name "${BOOT_ARCH}.config")
    if(NOT EXISTS "${boot_config_directory}/${boot_config_name}")
        set(boot_config_name "${BOOT_ARCH}_defconfig")
    endif()
    set(boot_config_names "${boot_config_name}")
endif()
set(BOOT_CONFIG_FILES)
foreach(boot_config_name IN LISTS boot_config_names)
    if(NOT boot_config_name MATCHES "^[A-Za-z0-9][A-Za-z0-9_.-]*$")
        message(FATAL_ERROR "Invalid BOOT_CONFIG file name: ${boot_config_name}")
    endif()
    set(boot_config_file "${boot_config_directory}/${boot_config_name}")
    if(NOT EXISTS "${boot_config_file}")
        message(FATAL_ERROR "Board configuration not found: ${boot_config_file}")
    endif()
    list(APPEND BOOT_CONFIG_FILES "${boot_config_file}")
endforeach()
list(JOIN boot_config_names "," BOOT_CONFIG)
set(BOOT_CONFIG "${BOOT_CONFIG}" CACHE STRING
    "Comma-separated board configuration file names" FORCE
)
set(BOOT_LOCAL_CONFIG_FILE "${CMAKE_BINARY_DIR}/kconfig.fragment")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/KConfig.cmake")
boot_load_kconfig()

# Payload configuration -----------------------------------------------------
if(CONFIG_BOOT AND (NOT CONFIG_BOOTLOADER1 OR NOT CONFIG_BOOTLOADER2))
    message(FATAL_ERROR "CONFIG_BOOT requires bootloader1 and bootloader2")
endif()
if(CONFIG_BOOTLOADER2_AUTO_BOOT AND CONFIG_PAYLOAD_ENTRY_ADDRESS STREQUAL "0")
    message(FATAL_ERROR
        "CONFIG_PAYLOAD_ENTRY_ADDRESS must be nonzero when payload handoff is enabled"
    )
endif()
if(BOOT_PAYLOAD_IMAGE)
    get_filename_component(BOOT_PAYLOAD_IMAGE "${BOOT_PAYLOAD_IMAGE}"
        ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if(NOT EXISTS "${BOOT_PAYLOAD_IMAGE}")
        message(FATAL_ERROR "BOOT_PAYLOAD_IMAGE does not exist: ${BOOT_PAYLOAD_IMAGE}")
    endif()
    if(NOT BOOT_PAYLOAD_FORMAT MATCHES "^(raw|elf)$")
        message(FATAL_ERROR "BOOT_PAYLOAD_FORMAT must be raw or elf")
    endif()
    if(BOOT_PAYLOAD_FORMAT STREQUAL "raw" AND
       CONFIG_PAYLOAD_LOAD_ADDRESS STREQUAL "0")
        message(FATAL_ERROR
            "CONFIG_PAYLOAD_LOAD_ADDRESS must be nonzero for a raw payload image"
        )
    endif()
endif()

# Platform selection ---------------------------------------------------------
if(BOOT_PLATFORM AND NOT CONFIG_PLATFORM_NAME STREQUAL BOOT_PLATFORM)
    message(FATAL_ERROR
        "CONFIG_PLATFORM_NAME=${CONFIG_PLATFORM_NAME} does not match ${BOOT_PLATFORM}"
    )
endif()
set(BOOT_PLATFORM "${CONFIG_PLATFORM_NAME}" CACHE STRING
    "Platform directory under platform/" FORCE
)
if(NOT BOOT_PLATFORM MATCHES "^[a-z0-9_-]+(/[a-z0-9_-]+)+$")
    message(FATAL_ERROR "Invalid CONFIG_PLATFORM_NAME: ${BOOT_PLATFORM}")
endif()
set(BOOT_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platform/${BOOT_PLATFORM}")
if(NOT EXISTS "${BOOT_PLATFORM_DIR}/platform.cmake")
    message(FATAL_ERROR "Platform not found: ${BOOT_PLATFORM_DIR}/platform.cmake")
endif()

# Configuration consistency checks ------------------------------------------
string(TOUPPER "${BOOT_ARCH}" boot_arch_config_suffix)
string(REPLACE "-" "_" boot_arch_config_suffix "${boot_arch_config_suffix}")
set(boot_arch_config "CONFIG_ARCH_${boot_arch_config_suffix}")
if(NOT ${boot_arch_config})
    message(FATAL_ERROR "Merged configuration does not enable ${boot_arch_config}")
endif()
set(boot_arch_config_count 0)
foreach(boot_arch_option IN ITEMS
        CONFIG_ARCH_AARCH64 CONFIG_ARCH_AARCH32 CONFIG_ARCH_CORTEX_M)
    if(${boot_arch_option})
        math(EXPR boot_arch_config_count "${boot_arch_config_count} + 1")
    endif()
endforeach()
if(NOT boot_arch_config_count EQUAL 1)
    message(FATAL_ERROR "Exactly one CONFIG_ARCH_* option must be enabled")
endif()
string(TOUPPER "${BOOT_PLATFORM}" boot_platform_config_suffix)
string(REGEX REPLACE "[/\\-]" "_" boot_platform_config_suffix
    "${boot_platform_config_suffix}"
)
set(boot_platform_config "CONFIG_PLATFORM_${boot_platform_config_suffix}")
if(NOT DEFINED ${boot_platform_config} OR NOT ${boot_platform_config})
    message(FATAL_ERROR "Merged configuration does not enable ${boot_platform_config}")
endif()
set(boot_platform_config_count 0)
foreach(boot_platform_option IN ITEMS
        CONFIG_PLATFORM_QEMU_VIRT
        CONFIG_PLATFORM_QEMU_VIRT_SECURE
        CONFIG_PLATFORM_QEMU_MPS2_AN385)
    if(${boot_platform_option})
        math(EXPR boot_platform_config_count "${boot_platform_config_count} + 1")
    endif()
endforeach()
if(NOT boot_platform_config_count EQUAL 1)
    message(FATAL_ERROR "Exactly one board CONFIG_PLATFORM_* option must be enabled")
endif()

# Platform settings ----------------------------------------------------------
include("${BOOT_PLATFORM_DIR}/platform.cmake")
if(BOOT_CONSOLE_DRIVER STREQUAL "pl011" AND NOT CONFIG_DRIVER_PL011)
    message(FATAL_ERROR "The selected platform requires CONFIG_DRIVER_PL011=y")
elseif(BOOT_CONSOLE_DRIVER STREQUAL "cmsdk_apb_uart" AND
       NOT CONFIG_DRIVER_CMSDK_APB_UART)
    message(FATAL_ERROR
        "The selected platform requires CONFIG_DRIVER_CMSDK_APB_UART=y"
    )
endif()
set(boot_console_driver_count 0)
foreach(boot_console_option IN ITEMS
        CONFIG_DRIVER_PL011 CONFIG_DRIVER_CMSDK_APB_UART)
    if(${boot_console_option})
        math(EXPR boot_console_driver_count "${boot_console_driver_count} + 1")
    endif()
endforeach()
if(NOT boot_console_driver_count EQUAL 1)
    message(FATAL_ERROR "Exactly one CONFIG_DRIVER_* console option must be enabled")
endif()

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
message(STATUS "Boot configurations: ${BOOT_CONFIG}")

# Debugger option ------------------------------------------------------------
set(BOOT_GDB_PORT "${CONFIG_GDB_PORT}")
if(NOT BOOT_GDB_PORT MATCHES "^[0-9]+$" OR BOOT_GDB_PORT LESS 1 OR BOOT_GDB_PORT GREATER 65535)
    message(FATAL_ERROR "BOOT_GDB_PORT must be between 1 and 65535")
endif()

# Memory override validation -------------------------------------------------
if(BOOT_MEMORY_CONFIG)
    get_filename_component(BOOT_MEMORY_CONFIG "${BOOT_MEMORY_CONFIG}"
        ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if(NOT EXISTS "${BOOT_MEMORY_CONFIG}")
        message(FATAL_ERROR "BOOT_MEMORY_CONFIG does not exist: ${BOOT_MEMORY_CONFIG}")
    endif()
endif()
