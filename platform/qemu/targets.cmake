# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/targets.cmake
# Brief: Define QEMU execution and debug-server targets.

# Emulator discovery ---------------------------------------------------------
find_program(BOOT_QEMU NAMES "${BOOT_QEMU_NAME}")

# QEMU image and execution arguments -----------------------------------------
list(APPEND BOOT_QEMU_ARGS
    -accel tcg
    -display none -monitor none -serial stdio
    -no-reboot -nic none
)
if(BOOT_QEMU_LOAD_MODE STREQUAL "bios")
    list(APPEND BOOT_QEMU_ARGS -bios "${PROJECT_BINARY_DIR}/boot.bin")
elseif(BOOT_QEMU_LOAD_MODE STREQUAL "kernel")
    list(APPEND BOOT_QEMU_ARGS -kernel "${BOOT_ELF}")
elseif(BOOT_QEMU_LOAD_MODE STREQUAL "loader")
    list(APPEND BOOT_QEMU_ARGS -device "loader,file=${BOOT_ELF},cpu-num=0")
else()
    message(FATAL_ERROR "Unknown platform QEMU load mode: ${BOOT_QEMU_LOAD_MODE}")
endif()

# QEMU run and debug targets -------------------------------------------------
if(BOOT_QEMU)
    add_custom_target(run
        COMMAND "${BOOT_QEMU}" ${BOOT_QEMU_ARGS}
        DEPENDS boot
        USES_TERMINAL
        VERBATIM
    )
    add_custom_target(debug
        COMMAND "${BOOT_QEMU}" ${BOOT_QEMU_ARGS}
                -S -gdb "tcp:127.0.0.1:${BOOT_GDB_PORT}"
        DEPENDS boot
        USES_TERMINAL
        VERBATIM
    )
else()
    message(STATUS "${BOOT_QEMU_NAME} not found: run/debug targets disabled")
endif()

