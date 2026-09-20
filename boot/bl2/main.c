/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/bl2/main.c
 * Brief: Validate bootloader2 and optionally transfer control to a payload.
 */

/* Includes --------------------------------------------------------------- */
#include "boot.h"
#include "driver/console.h"

/* Bootloader2 entry ------------------------------------------------------ */
void boot_main(void)
{
    boot_stage_run("bootloader2");
#if defined(CONFIG_BOOTLOADER2_AUTO_BOOT)
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
