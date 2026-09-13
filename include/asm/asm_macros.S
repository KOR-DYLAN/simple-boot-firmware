/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/asm/asm_macros.S
 * Brief: Select common and architecture-specific assembly helpers.
 */

#ifndef SIMPLE_BOOT_ASM_MACROS_S
#define SIMPLE_BOOT_ASM_MACROS_S

/* Architecture selection validation -------------------------------------- */
#if (defined(BOOT_ARCH_AARCH64) + defined(BOOT_ARCH_AARCH32) + defined(BOOT_ARCH_CORTEX_M)) != 1
#error "Select exactly one BOOT_ARCH for the assembly macro layer"
#endif

/* Includes --------------------------------------------------------------- */
#include "asm/common.S"
#if defined(BOOT_ARCH_AARCH64)
#include "asm/aarch64.S"
#elif defined(BOOT_ARCH_AARCH32)
#include "asm/aarch32.S"
#else
#include "asm/cortex_m.S"
#endif

#endif /* SIMPLE_BOOT_ASM_MACROS_S */
