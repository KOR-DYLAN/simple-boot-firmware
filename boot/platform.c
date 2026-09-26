/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/platform.c
 * Brief: Provide default platform initialization hooks.
 */

/* Includes --------------------------------------------------------------- */
#include "platform.h"
#include "compiler_rt.h"

/* Weak platform hooks ---------------------------------------------------- */
/*
 * Keep these external definitions in one translation unit. Header-local
 * functions cannot be replaced reliably by a platform's strong definition.
 */
COMPILER_WEAK void platform_early_init(void)
{
}

COMPILER_WEAK void platform_arch_init(void)
{
}

COMPILER_WEAK void platform_init(void)
{
}

COMPILER_WEAK void platform_handoff(void)
{
}
