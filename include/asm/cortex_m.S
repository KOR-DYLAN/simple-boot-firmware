/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/asm/cortex_m.S
 * Brief: Define Cortex-M vector entries and weak handler aliases.
 */

/* ARMv7-M Thumb-2 helper macros. */
#ifndef SIMPLE_BOOT_ASM_CORTEX_M_S
#define SIMPLE_BOOT_ASM_CORTEX_M_S

/* Includes --------------------------------------------------------------- */
#include "asm/aarch32.S"

/* Exception vector macros ------------------------------------------------ */
/* A strong application handler overrides this Thumb-aware default alias. */
.macro vector_handler name:req, default=default_exception_handler
    .word \name
    weak_func_alias \name, \default
.endm

#endif /* SIMPLE_BOOT_ASM_CORTEX_M_S */
