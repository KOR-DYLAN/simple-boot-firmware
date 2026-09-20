# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/KConfig.cmake
# Brief: Load KConfig settings and provide configuration-aware build helpers.

# KConfig generation ---------------------------------------------------------
function(boot_load_kconfig)
    find_program(Python3_EXECUTABLE NAMES python3 python)
    if(NOT Python3_EXECUTABLE)
        message(FATAL_ERROR "Python is required to parse KConfig files")
    endif()

    set(BOOT_KCONFIG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated" CACHE INTERNAL
        "Generated KConfig output directory"
    )
    set(boot_config_arguments)
    foreach(boot_config_file IN LISTS BOOT_CONFIG_FILES)
        list(APPEND boot_config_arguments --config "${boot_config_file}")
    endforeach()
    if(EXISTS "${BOOT_LOCAL_CONFIG_FILE}")
        list(APPEND boot_config_arguments --config "${BOOT_LOCAL_CONFIG_FILE}")
    endif()
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/kconfig.py"
                --source-dir "${CMAKE_SOURCE_DIR}"
                --kconfig "${CMAKE_SOURCE_DIR}/KConfig"
                ${boot_config_arguments}
                --output-dir "${BOOT_KCONFIG_OUTPUT_DIR}"
        RESULT_VARIABLE boot_kconfig_result
        OUTPUT_VARIABLE boot_kconfig_output
        ERROR_VARIABLE boot_kconfig_error
    )
    if(NOT boot_kconfig_result EQUAL 0)
        message(FATAL_ERROR "KConfig parsing failed:\n${boot_kconfig_error}")
    endif()

    include("${BOOT_KCONFIG_OUTPUT_DIR}/config.cmake")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        ${BOOT_CONFIG_FILES}
        "${BOOT_LOCAL_CONFIG_FILE}"
        "${CMAKE_SOURCE_DIR}/cmake/kconfig.py"
        ${BOOT_KCONFIG_FILES}
    )
    set(BOOT_AUTOCONF_HEADER "${BOOT_KCONFIG_OUTPUT_DIR}/autoconf.h" PARENT_SCOPE)
    foreach(boot_config_name IN LISTS BOOT_KCONFIG_SYMBOLS)
        set("${boot_config_name}" "${${boot_config_name}}" PARENT_SCOPE)
    endforeach()
endfunction()

# Conditional composition ----------------------------------------------------
define_property(DIRECTORY PROPERTY BOOT_SOURCE_TARGET INHERITED
    BRIEF_DOCS "Target that receives sources from this directory tree"
    FULL_DOCS "Aggregate target inherited by add_source_if_enabled calls"
)

function(boot_require_directory directory)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
        message(FATAL_ERROR "Missing ${directory}/CMakeLists.txt")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/KConfig")
        message(FATAL_ERROR "Missing ${directory}/KConfig")
    endif()
endfunction()

macro(add_subdirectory_if_enabled config directory)
    if(NOT DEFINED ${config})
        message(FATAL_ERROR "Unknown configuration symbol: ${config}")
    endif()
    if(${config})
        boot_require_directory("${directory}")
        add_subdirectory("${directory}")
    endif()
endmacro()

function(add_library_if_enabled config target type)
    if(NOT DEFINED ${config})
        message(FATAL_ERROR "Unknown configuration symbol: ${config}")
    endif()
    if(${config})
        add_library("${target}" "${type}")
        set_property(DIRECTORY PROPERTY BOOT_SOURCE_TARGET "${target}")
        if(ARGN)
            target_sources("${target}" PRIVATE ${ARGN})
        endif()
    endif()
endfunction()

function(add_executable_if_enabled config target)
    if(NOT DEFINED ${config})
        message(FATAL_ERROR "Unknown configuration symbol: ${config}")
    endif()
    if(${config})
        add_executable("${target}")
        if(ARGN)
            target_sources("${target}" PRIVATE ${ARGN})
        endif()
    endif()
endfunction()

function(set_source_target target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Source target does not exist: ${target}")
    endif()
    set_property(DIRECTORY PROPERTY BOOT_SOURCE_TARGET "${target}")
endfunction()

function(add_source_if_enabled config)
    if(NOT DEFINED ${config})
        message(FATAL_ERROR "Unknown configuration symbol: ${config}")
    endif()
    if(${config})
        get_property(boot_source_target DIRECTORY PROPERTY BOOT_SOURCE_TARGET)
        if(NOT boot_source_target OR NOT TARGET "${boot_source_target}")
            message(FATAL_ERROR
                "No aggregate source target is available for ${CMAKE_CURRENT_SOURCE_DIR}"
            )
        endif()
        target_sources("${boot_source_target}" PRIVATE ${ARGN})
    endif()
endfunction()

function(target_link_libraries_if_enabled config target scope)
    if(NOT DEFINED ${config})
        message(FATAL_ERROR "Unknown configuration symbol: ${config}")
    endif()
    if(${config})
        target_link_libraries("${target}" "${scope}" ${ARGN})
    endif()
endfunction()
