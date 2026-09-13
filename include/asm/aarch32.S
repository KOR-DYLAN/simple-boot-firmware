/*
 * Copyright (c) 2016-2026, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * File: include/asm/aarch32.S
 * Brief: Define ARM and Thumb-2 constant, address, and CP15 helpers.
 */

#ifndef SIMPLE_BOOT_ASM_AARCH32_S
#define SIMPLE_BOOT_ASM_AARCH32_S

/* Includes --------------------------------------------------------------- */
#include "arch/arch_def.h"

/* Constant loading macros ------------------------------------------------ */
/*
 * Load a signed or unsigned 32-bit constant without a literal pool. MOVW writes the
 * low halfword; MOVT writes the high halfword if needed. Both preserve condition
 * flags. Use r0-r12 or lr as the destination; initialize SP through a temporary
 * general-purpose register.
 */
.macro mov_imm reg:req, value:req
    .if ((\value) < ASM_SIGNED_WORD_MIN) || ((\value) > ASM_UNSIGNED_WORD_MAX)
        .error "mov_imm requires a signed or unsigned 32-bit constant"
    .endif
    movw    \reg, #((\value) & ASM_HALFWORD_MASK)
    .if ((\value) & ASM_WORD_UPPER_MASK) != 0
        movt    \reg, #(((\value) >> ASM_HALFWORD_BITS) & ASM_HALFWORD_MASK)
    .endif
.endm

/* Symbol address macros -------------------------------------------------- */
/* Load a link-time absolute address from a symbol or symbol + offset. */
.macro load_addr reg:req, symbol:req
    movw    \reg, #:lower16:\symbol
    movt    \reg, #:upper16:\symbol
.endm

#if defined(BOOT_ARCH_AARCH32)

/* Coprocessor access macros ---------------------------------------------- */
/* Use a named CP15 tuple: coprocessor, opc1, CRn, CRm, opc2. */
.macro ldcopr reg:req, coproc:req, opc1:req, crn:req, crm:req, opc2:req
    mrc     \coproc, \opc1, \reg, \crn, \crm, \opc2
.endm

.macro stcopr reg:req, coproc:req, opc1:req, crn:req, crm:req, opc2:req
    mcr     \coproc, \opc1, \reg, \crn, \crm, \opc2
.endm
#endif

#endif /* SIMPLE_BOOT_ASM_AARCH32_S */
