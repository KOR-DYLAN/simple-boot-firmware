/*
 * Copyright (c) 2013-2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * File: include/asm/common.S
 * Brief: Define function sections, linkage, aliases, and unwind boundaries.
 */

#ifndef SIMPLE_BOOT_ASM_COMMON_S
#define SIMPLE_BOOT_ASM_COMMON_S

/* Includes --------------------------------------------------------------- */
#include "arch/arch_def.h"

/* Function definition macros --------------------------------------------- */
/*
 * Alignment is log2(bytes). Linkage is global, local, or weak. Reset code selects
 * .text.boot, which the linker retains; ordinary functions use individual sections.
 */
.macro func name:req, align=ARCH_FUNCTION_ALIGN_SHIFT, linkage=global, section=
#if defined(BOOT_ARCH_CORTEX_M)
    .if \align < ARCH_THUMB_ALIGN_SHIFT
#else
    .if \align < ARCH_FUNCTION_ALIGN_SHIFT
#endif
        .error "func alignment is smaller than the instruction alignment"
    .endif
    .ifb \section
        .section .text.asm.\name, "ax", %progbits
    .else
        .section \section, "ax", %progbits
    .endif
    .p2align \align
    .ifc \linkage,global
        .global \name
    .else
        .ifc \linkage,weak
            .weak \name
        .else
            .ifnc \linkage,local
                .error "func linkage must be global, local, or weak"
            .else
                .local \name
            .endif
        .endif
    .endif
    .type \name, %function
#if defined(BOOT_ARCH_CORTEX_M)
    .thumb_func
#endif
\name:
    .cfi_sections .debug_frame
    .cfi_startproc
    .cfi_def_cfa sp, ASM_INITIAL_CFA_OFFSET
.endm

.macro endfunc name:req
    .cfi_endproc
    .size \name, . - \name
.endm

/* Unwind boundary macro -------------------------------------------------- */
/* Stop unwinding at reset and stackless exception entry points. */
.macro unwind_root
#if defined(BOOT_ARCH_AARCH64)
    .cfi_undefined x30
#else
    .cfi_undefined lr
#endif
.endm

/* Function alias macros -------------------------------------------------- */
.macro _func_alias alias:req, target:req
    .type \alias, %function
#if defined(BOOT_ARCH_CORTEX_M)
    .thumb_set \alias, \target
#else
    .set \alias, \target
#endif
.endm

.macro func_alias alias:req, target:req
    .global \alias
    _func_alias \alias, \target
.endm

.macro weak_func_alias alias:req, target:req
    .weak \alias
    _func_alias \alias, \target
.endm

#endif /* SIMPLE_BOOT_ASM_COMMON_S */
