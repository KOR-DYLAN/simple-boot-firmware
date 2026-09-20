/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/bl1/main.c
 * Brief: Validate bootloader1 and transfer control to bootloader2.
 */

/* Includes --------------------------------------------------------------- */
#include "boot.h"
#include "boot_stage.h"
#include "memory_layout.h"

/* Bootloader1 entry ------------------------------------------------------ */
void boot_main(void)
{
    boot_stage_run("bootloader1");
    boot_print_handoff("Jump to bootloader2: ", BOOTLOADER2_ENTRY_ADDRESS);
    boot_jump(BOOTLOADER2_ENTRY_ADDRESS,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE);
}
