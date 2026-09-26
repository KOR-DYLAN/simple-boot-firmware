/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/main.c
 * Brief: Run the selected boot stage and transfer control when configured.
 */

/* Includes --------------------------------------------------------------- */
#include "boot.h"
#include "boot_stage.h"
#include "driver/console.h"
#include "memory_layout.h"
#include "platform.h"

/* Stage helpers ---------------------------------------------------------- */
#if BOOT_STAGE == BOOT_STAGE_1
#define BOOT_STAGE_NAME "bootloader1"
#else
#define BOOT_STAGE_NAME "bootloader2"
#endif

static void boot_handoff_next(void)
{
#if BOOT_STAGE == BOOT_STAGE_1
    boot_print_handoff("Jump to bootloader2: ", BOOTLOADER2_ENTRY_ADDRESS);
    boot_jump(BOOTLOADER2_ENTRY_ADDRESS,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE,
              BOOT_HANDOFF_ARGUMENT_NONE);
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

/* Boot entry ------------------------------------------------------------- */
void boot_main(void)
{
    platform_early_init();
    boot_stage_run(BOOT_STAGE_NAME);
    platform_arch_init();
    platform_init();
    boot_handoff_next();
}
