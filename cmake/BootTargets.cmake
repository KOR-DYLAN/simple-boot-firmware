# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/BootTargets.cmake
# Brief: Load platform execution targets and define the GDB client.

# Platform execution targets -------------------------------------------------
set(BOOT_ELF "${PROJECT_BINARY_DIR}/boot.elf")
if(BOOT_PLATFORM_TARGETS)
    include("${BOOT_PLATFORM_TARGETS}")
endif()

# Debugger discovery ---------------------------------------------------------
find_program(BOOT_GDB NAMES "${CROSS_COMPILE}gdb" gdb-multiarch)

# GDB script and connection target -------------------------------------------
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/boot.gdb.in"
    "${PROJECT_BINARY_DIR}/boot.gdb"
    @ONLY
)
if(BOOT_GDB)
    add_custom_target(gdb
        COMMAND "${BOOT_GDB}" -nx -x "${PROJECT_BINARY_DIR}/boot.gdb"
        DEPENDS boot
        USES_TERMINAL
        VERBATIM
    )
else()
    message(STATUS "Cross GDB not found: gdb target disabled; set BOOT_GDB to override")
endif()
