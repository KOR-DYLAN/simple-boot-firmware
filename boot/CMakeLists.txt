# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: boot/CMakeLists.txt
# Brief: Build boot stages and generate their artifacts.

# Linker preprocessing inputs ------------------------------------------------
# Preprocess each linker script with its stage and the shared build settings.
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

# Stage construction ---------------------------------------------------------
function(add_boot_stage stage image_name)
    set(boot_target "${image_name}_image")
    set(boot_linker_script "${PROJECT_BINARY_DIR}/${image_name}.ld")
    set(boot_map "${PROJECT_BINARY_DIR}/${image_name}.map")
    set(boot_stage_sources
        "${BOOT_SOURCE_DIR}/main.c"
        "${BOOT_SOURCE_DIR}/platform.c"
        ${BOOT_PLATFORM_SOURCES}
    )
    if(stage EQUAL BOOT_STAGE_1)
        list(APPEND boot_stage_sources
            ${BOOT_PLATFORM_STAGE1_SOURCES}
            ${BOOT_PLATFORM_BL1_SOURCES}
        )
    else()
        list(APPEND boot_stage_sources
            ${BOOT_PLATFORM_STAGE2_SOURCES}
            ${BOOT_PLATFORM_BL2_SOURCES}
        )
    endif()

    add_custom_command(
        OUTPUT "${boot_linker_script}"
        COMMAND "${CMAKE_C_COMPILER}" ${BOOT_CPU_FLAGS} ${BOOT_CPP_DEFINES}
                ${BOOT_CPP_INCLUDES} -nostdinc "-DBOOT_STAGE=${stage}"
                -include "${BOOT_AUTOCONF_HEADER}" -E -P -x c
                -MMD -MF "${boot_linker_script}.d" -MT "${boot_linker_script}"
                "${BOOT_SOURCE_DIR}/boot.ld.S" -o "${boot_linker_script}"
        DEPENDS
            "${BOOT_SOURCE_DIR}/boot.ld.S"
            "${BOOT_COMMON_SOURCE}"
            "${PROJECT_SOURCE_DIR}/include/boot_stage.h"
            "${PROJECT_SOURCE_DIR}/include/memory_layout.h"
            "${BOOT_AUTOCONF_HEADER}"
            "${BOOT_PLATFORM_DIR}/include/platform_memory.h"
            ${BOOT_MEMORY_CONFIG}
        DEPFILE "${boot_linker_script}.d"
        VERBATIM
    )
    add_custom_target("${image_name}_linker_script" DEPENDS "${boot_linker_script}")

    add_executable("${boot_target}"
        "${BOOT_COMMON_SOURCE}"
        ${boot_stage_sources}
        ${ARGN}
    )
    set_target_properties("${boot_target}" PROPERTIES
        OUTPUT_NAME "${image_name}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        SUFFIX .elf
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
    )
    target_compile_definitions("${boot_target}" PRIVATE "BOOT_STAGE=${stage}")
    target_link_libraries_if_enabled(
        CONFIG_BOOT "${boot_target}" PRIVATE
        boot_options boot_arch liblibrary libdriver
    )
    target_link_options("${boot_target}" PRIVATE
        ${BOOT_CPU_FLAGS}
        -nostdlib -nostartfiles -static -no-pie
        "-T${boot_linker_script}"
        "-Wl,-Map,${boot_map}"
        -Wl,--gc-sections -Wl,--build-id=none -Wl,-z,noexecstack
    )
    add_dependencies("${boot_target}" "${image_name}_linker_script")
    set_property(TARGET "${boot_target}" APPEND PROPERTY LINK_DEPENDS
        "${boot_linker_script}"
    )

    set(boot_byproducts
        "${PROJECT_BINARY_DIR}/${image_name}.bin"
        "${PROJECT_BINARY_DIR}/${image_name}.hex"
        "${PROJECT_BINARY_DIR}/${image_name}.asm"
        "${PROJECT_BINARY_DIR}/${image_name}-memory-map.svg"
        "${PROJECT_BINARY_DIR}/${image_name}-memory-map.html"
        "${PROJECT_BINARY_DIR}/${image_name}-memory-map.json"
    )
    if(stage EQUAL BOOT_STAGE_1)
        list(APPEND boot_byproducts "${PROJECT_BINARY_DIR}/clangd.config")
    endif()
    add_custom_target("${image_name}"
        COMMAND "${Python3_EXECUTABLE}" "${BOOT_ARTIFACT_GENERATOR}"
                --source-dir "${PROJECT_SOURCE_DIR}"
                --build-dir "${PROJECT_BINARY_DIR}"
                --elf "$<TARGET_FILE:${boot_target}>"
                --image-name "${image_name}"
                --arch "${BOOT_ARCH}"
                --objcopy "${CMAKE_OBJCOPY}"
                --objdump "${CMAKE_OBJDUMP}"
                --readelf "${CMAKE_READELF}"
        COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:${boot_target}>"
        DEPENDS "${boot_target}" "${BOOT_ARTIFACT_GENERATOR}"
        BYPRODUCTS ${boot_byproducts}
        VERBATIM
    )
endfunction()

set(BOOT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BOOT_COMMON_SOURCE "${BOOT_SOURCE_DIR}/common.c")
set(BOOT_STAGE_1 1)
set(BOOT_STAGE_2 2)
if(CONFIG_BOOTLOADER1)
    add_boot_stage(${BOOT_STAGE_1} bootloader1)
endif()
if(CONFIG_BOOTLOADER2)
    add_boot_stage(${BOOT_STAGE_2} bootloader2)
endif()

# Combined image and aggregate target ----------------------------------------
set(BOOT_FIRMWARE_BIN "${PROJECT_BINARY_DIR}/firmware.bin")
add_custom_target(firmware
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/combine_images.py"
            --readelf "${CMAKE_READELF}"
            --first-elf "${PROJECT_BINARY_DIR}/bootloader1.elf"
            --first-bin "${PROJECT_BINARY_DIR}/bootloader1.bin"
            --second-elf "${PROJECT_BINARY_DIR}/bootloader2.elf"
            --second-bin "${PROJECT_BINARY_DIR}/bootloader2.bin"
            --output "${BOOT_FIRMWARE_BIN}"
    DEPENDS bootloader1 bootloader2 "${PROJECT_SOURCE_DIR}/cmake/combine_images.py"
    BYPRODUCTS "${BOOT_FIRMWARE_BIN}"
    VERBATIM
)
add_custom_target(boot ALL DEPENDS firmware)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${PROJECT_BINARY_DIR}/bootloader1.map"
    "${PROJECT_BINARY_DIR}/bootloader2.map"
)

# Memory layout inspection ---------------------------------------------------
add_custom_target(layout
    COMMAND "${CMAKE_READELF}" -W -l -S "$<TARGET_FILE:bootloader1_image>"
    COMMAND "${CMAKE_READELF}" -W -l -S "$<TARGET_FILE:bootloader2_image>"
    DEPENDS boot
    USES_TERMINAL
    VERBATIM
)

# Execution and debugger targets --------------------------------------------
set(BOOTLOADER1_ELF "${PROJECT_BINARY_DIR}/bootloader1.elf")
set(BOOTLOADER1_BIN "${PROJECT_BINARY_DIR}/bootloader1.bin")
set(BOOTLOADER2_ELF "${PROJECT_BINARY_DIR}/bootloader2.elf")
include("${PROJECT_SOURCE_DIR}/cmake/BootTargets.cmake")
