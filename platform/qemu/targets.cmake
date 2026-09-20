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
    list(APPEND BOOT_QEMU_ARGS -bios "${BOOT_FIRMWARE_BIN}")
elseif(BOOT_QEMU_LOAD_MODE STREQUAL "kernel")
    list(APPEND BOOT_QEMU_ARGS -kernel "${BOOTLOADER1_ELF}")
elseif(BOOT_QEMU_LOAD_MODE STREQUAL "loader")
    list(APPEND BOOT_QEMU_ARGS
        -device "loader,file=${BOOTLOADER1_ELF},cpu-num=0"
    )
else()
    message(FATAL_ERROR "Unknown platform QEMU load mode: ${BOOT_QEMU_LOAD_MODE}")
endif()
if(NOT BOOT_QEMU_LOAD_MODE STREQUAL "bios")
    list(APPEND BOOT_QEMU_ARGS -device "loader,file=${BOOTLOADER2_ELF}")
endif()
if(BOOT_PAYLOAD_IMAGE)
    if(BOOT_PAYLOAD_FORMAT STREQUAL "raw")
        list(APPEND BOOT_QEMU_ARGS
            -device "loader,file=${BOOT_PAYLOAD_IMAGE},addr=${CONFIG_PAYLOAD_LOAD_ADDRESS},force-raw=on"
        )
    else()
        list(APPEND BOOT_QEMU_ARGS -device "loader,file=${BOOT_PAYLOAD_IMAGE}")
    endif()
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
