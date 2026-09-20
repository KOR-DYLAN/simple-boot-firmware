# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: boot/CMakeLists.txt
# Brief: Preprocess the linker script and generate boot images.

# Linker preprocessing inputs ------------------------------------------------
# Preprocess the linker script with the same headers and definitions as C.
get_target_property(boot_include_dirs boot_options INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(boot_defines boot_options INTERFACE_COMPILE_DEFINITIONS)
set(BOOT_CPP_INCLUDES)
foreach(boot_include IN LISTS boot_include_dirs)
    list(APPEND BOOT_CPP_INCLUDES -I "${boot_include}")
endforeach()
set(BOOT_CPP_DEFINES)
foreach(boot_define IN LISTS boot_defines)
    list(APPEND BOOT_CPP_DEFINES "-D${boot_define}")
endforeach()

# Linker script generation ---------------------------------------------------
set(BOOT_LINKER_SCRIPT "${PROJECT_BINARY_DIR}/boot.ld")
add_custom_command(
    OUTPUT "${BOOT_LINKER_SCRIPT}"
    COMMAND "${CMAKE_C_COMPILER}" ${BOOT_CPU_FLAGS} ${BOOT_CPP_DEFINES}
            ${BOOT_CPP_INCLUDES} -nostdinc
            -include "${BOOT_AUTOCONF_HEADER}" -E -P -x c
            -MMD -MF "${PROJECT_BINARY_DIR}/boot.ld.d" -MT "${BOOT_LINKER_SCRIPT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/boot.ld.S" -o "${BOOT_LINKER_SCRIPT}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/boot.ld.S"
        "${PROJECT_SOURCE_DIR}/include/memory_layout.h"
        "${BOOT_AUTOCONF_HEADER}"
        "${BOOT_PLATFORM_DIR}/include/platform_memory.h"
        ${BOOT_MEMORY_CONFIG}
    DEPFILE "${PROJECT_BINARY_DIR}/boot.ld.d"
    VERBATIM
)
add_custom_target(boot_linker_script DEPENDS "${BOOT_LINKER_SCRIPT}")

# Boot executable ------------------------------------------------------------
add_executable_if_enabled(CONFIG_BOOT boot_image main.c)
set_target_properties(boot_image PROPERTIES
    OUTPUT_NAME boot
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    SUFFIX .elf
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)
target_link_libraries_if_enabled(
    CONFIG_BOOT boot_image PRIVATE
    boot_options boot_arch liblibrary libdriver
)

# Link options and dependencies ----------------------------------------------
target_link_options(boot_image PRIVATE
    ${BOOT_CPU_FLAGS}
    -nostdlib -nostartfiles -static -no-pie
    "-T${BOOT_LINKER_SCRIPT}"
    "-Wl,-Map,${PROJECT_BINARY_DIR}/boot.map"
    -Wl,--gc-sections -Wl,--build-id=none -Wl,-z,noexecstack
)
add_dependencies(boot_image boot_linker_script)
set_property(TARGET boot_image APPEND PROPERTY LINK_DEPENDS "${BOOT_LINKER_SCRIPT}")

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${PROJECT_BINARY_DIR}/boot.map"
)

# Image and report generation ------------------------------------------------
# This target runs even when the ELF does not need to be relinked.
add_custom_target(boot ALL
    COMMAND "${Python3_EXECUTABLE}" "${BOOT_ARTIFACT_GENERATOR}"
            --source-dir "${PROJECT_SOURCE_DIR}" --build-dir "${PROJECT_BINARY_DIR}"
            --arch "${BOOT_ARCH}" --objcopy "${CMAKE_OBJCOPY}"
            --objdump "${CMAKE_OBJDUMP}" --readelf "${CMAKE_READELF}"
    COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:boot_image>"
    DEPENDS boot_image "${BOOT_ARTIFACT_GENERATOR}"
    BYPRODUCTS
        "${PROJECT_BINARY_DIR}/boot.bin"
        "${PROJECT_BINARY_DIR}/boot.hex"
        "${PROJECT_BINARY_DIR}/boot.asm"
        "${PROJECT_BINARY_DIR}/memory-map.svg"
        "${PROJECT_BINARY_DIR}/memory-map.html"
        "${PROJECT_BINARY_DIR}/memory-map.json"
        "${PROJECT_BINARY_DIR}/clangd.config"
    VERBATIM
)

# Memory layout inspection ---------------------------------------------------
add_custom_target(layout
    COMMAND "${CMAKE_READELF}" -W -l -S "$<TARGET_FILE:boot_image>"
    DEPENDS boot
    USES_TERMINAL
    VERBATIM
)

# Execution and debugger targets ---------------------------------------------
include("${PROJECT_SOURCE_DIR}/cmake/BootTargets.cmake")
