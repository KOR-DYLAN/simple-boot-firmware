/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/main.c
 * Brief: Initialize the platform and run the configured boot image.
 */

/* Includes --------------------------------------------------------------- */
#include "boot.h"
#include "memory_layout.h"
#include "platform.h"

/* Boot entry ------------------------------------------------------------- */
/*
 * Assembly enters here only after .data, .bss, and the stack are ready.
 * Platform hooks remain ordered from the least initialized environment to
 * the final ownership transfer; a later hook may rely on every earlier step.
 */
void boot_main(void)
{
    /* Perform work required before the common console can be initialized. */
    platform_early_init();

    /* Initialize diagnostics and validate the C runtime and memory layout. */
    boot_run(PLATFORM_IMAGE_NAME);

    /* Configure architectural state before board devices and services. */
    platform_arch_init();
    platform_init();

    /* Transfer control when configured, or return to the assembly halt path. */
    platform_handoff();
}
