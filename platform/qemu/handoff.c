/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/handoff.c
 * Brief: Transfer control from a QEMU boot image to its configured successor.
 */

/* Includes --------------------------------------------------------------- */
#include "boot.h"
#include "compiler_rt.h"
#include "driver/console.h"
#include "memory_layout.h"
#include "platform.h"

/* Platform handoff ------------------------------------------------------- */
#define QEMU_HANDOFF_ARGUMENT_NONE U(0)

void platform_handoff(void)
{
#if PLATFORM_IMAGE == 1
    boot_print_handoff("Jump to bootloader2: ", BOOTLOADER2_ENTRY_ADDRESS);
    boot_jump(BOOTLOADER2_ENTRY_ADDRESS,
              QEMU_HANDOFF_ARGUMENT_NONE,
              QEMU_HANDOFF_ARGUMENT_NONE,
              QEMU_HANDOFF_ARGUMENT_NONE,
              QEMU_HANDOFF_ARGUMENT_NONE);
#elif defined(CONFIG_BOOTLOADER2_AUTO_BOOT)
    boot_print_handoff("Jump to payload: ", CONFIG_PAYLOAD_ENTRY_ADDRESS);
    boot_jump(CONFIG_PAYLOAD_ENTRY_ADDRESS,
              CONFIG_PAYLOAD_ARGUMENT_0,
              CONFIG_PAYLOAD_ARGUMENT_1,
              CONFIG_PAYLOAD_ARGUMENT_2,
              CONFIG_PAYLOAD_ARGUMENT_3);
#else
    console_puts("Payload handoff disabled\n");
#endif
}
