/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/platform.c
 * Brief: Provide default platform initialization hooks.
 */

/* Includes --------------------------------------------------------------- */
#include "platform.h"

/* Weak platform hooks ---------------------------------------------------- */
__attribute__((weak)) void platform_early_init(void)
{
}

__attribute__((weak)) void platform_arch_init(void)
{
}

__attribute__((weak)) void platform_init(void)
{
}
