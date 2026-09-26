# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: boot/CMakeLists.txt
# Brief: Build configured boot images and generate their artifacts.

# Linker preprocessing inputs ------------------------------------------------
# The C preprocessor resolves architecture, platform, and selected-image memory
# definitions before GNU ld reads the script.
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

# Selected image -------------------------------------------------------------
if(CONFIG_BOOTLOADER1)
    set(BOOT_IMAGE_NAME bootloader1)
    set(BOOT_IMAGE_DEFINES ${BOOT_PLATFORM_IMAGE1_DEFINES})
    set(BOOT_IMAGE_PLATFORM_SOURCES
        ${BOOT_PLATFORM_STAGE1_SOURCES}
        ${BOOT_PLATFORM_BL1_SOURCES}
    )
else()
    set(BOOT_IMAGE_NAME bootloader2)
    set(BOOT_IMAGE_DEFINES ${BOOT_PLATFORM_IMAGE2_DEFINES})
    set(BOOT_IMAGE_PLATFORM_SOURCES
        ${BOOT_PLATFORM_STAGE2_SOURCES}
        ${BOOT_PLATFORM_BL2_SOURCES}
    )
endif()

# Image construction ---------------------------------------------------------
function(add_boot_image image_name)
    set(boot_target "${image_name}_image")
    set(boot_linker_script "${PROJECT_BINARY_DIR}/${image_name}.ld")
    set(boot_map "${PROJECT_BINARY_DIR}/${image_name}.map")
    set(boot_image_sources
        "${BOOT_SOURCE_DIR}/main.c"
        "${BOOT_SOURCE_DIR}/panic.c"
        "${BOOT_SOURCE_DIR}/platform.c"
        ${BOOT_PLATFORM_SOURCES}
        ${BOOT_IMAGE_PLATFORM_SOURCES}
    )

    set(boot_image_cpp_defines)
    foreach(boot_image_define IN LISTS BOOT_IMAGE_DEFINES)
        list(APPEND boot_image_cpp_defines "-D${boot_image_define}")
    endforeach()

    add_custom_command(
        OUTPUT "${boot_linker_script}"
        COMMAND "${CMAKE_C_COMPILER}" ${BOOT_CPU_FLAGS} ${BOOT_CPP_DEFINES}
                ${BOOT_CPP_INCLUDES} ${boot_image_cpp_defines} -nostdinc
                -D__LINKER__=1
                -include "${BOOT_AUTOCONF_HEADER}" -E -P -x c
                -MMD -MF "${boot_linker_script}.d" -MT "${boot_linker_script}"
                "${BOOT_SOURCE_DIR}/boot.ld.S" -o "${boot_linker_script}"
        DEPENDS
            "${BOOT_SOURCE_DIR}/boot.ld.S"
            "${BOOT_COMMON_SOURCE}"
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
        ${boot_image_sources}
        ${ARGN}
    )
    set_target_properties("${boot_target}" PROPERTIES
        OUTPUT_NAME "${image_name}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        SUFFIX .elf
        C_STANDARD 99
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
    )
    target_compile_definitions("${boot_target}" PRIVATE ${BOOT_IMAGE_DEFINES})
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
        "${PROJECT_BINARY_DIR}/clangd.config"
    )
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
add_boot_image("${BOOT_IMAGE_NAME}")
add_custom_target(boot ALL DEPENDS "${BOOT_IMAGE_NAME}")
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${PROJECT_BINARY_DIR}/${BOOT_IMAGE_NAME}.map"
)

# Memory layout inspection ---------------------------------------------------
add_custom_target(layout
    COMMAND "${CMAKE_READELF}" -W -l -S
            "$<TARGET_FILE:${BOOT_IMAGE_NAME}_image>"
    DEPENDS boot
    USES_TERMINAL
    VERBATIM
)

# Execution and debugger targets ---------------------------------------------
# Only the first image owns execution targets. Make supplies the multibuild
# root so these targets can locate the independently configured second image.
if(CONFIG_BOOTLOADER1 AND BOOT_MULTIBUILD_ROOT)
    set(BOOTLOADER1_ELF "${BOOT_BL1_BUILD_DIR}/bootloader1.elf")
    set(BOOTLOADER1_BIN "${BOOT_BL1_BUILD_DIR}/bootloader1.bin")
    set(BOOTLOADER2_ELF "${BOOT_BL2_BUILD_DIR}/bootloader2.elf")
    set(BOOT_FIRMWARE_BIN "${BOOT_MULTIBUILD_ROOT}/firmware.bin")
    include("${PROJECT_SOURCE_DIR}/cmake/BootTargets.cmake")
endif()
