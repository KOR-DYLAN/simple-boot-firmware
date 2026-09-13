/*
 * Copyright (c) 2013-2026, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * File: include/asm/aarch64.S
 * Brief: Define AArch64 constant, address, and exception-vector helpers.
 */

#ifndef SIMPLE_BOOT_ASM_AARCH64_S
#define SIMPLE_BOOT_ASM_AARCH64_S

/* Includes --------------------------------------------------------------- */
#include "arch/arch_def.h"

/* Constant loading macros ------------------------------------------------ */
/*
 * Load an assembly-time 64-bit integer expression without a literal pool. W registers
 * accept signed or unsigned 32-bit values. Negative values use two's complement at the
 * destination width. Use X/W register names; load_addr handles ELF symbol addresses.
 * MOVZ initializes the first nonzero halfword; MOVK inserts the remaining nonzero
 * halfwords.
 */
.macro mov_imm reg:req, val:req
    .set .Lmov_width\@, ASM_DWORD_BITS
    .irp index, A64_W_REGISTER_SUFFIXES
        .ifc \reg,w\index
            .set .Lmov_width\@, ASM_WORD_BITS
        .endif
        .ifc \reg,W\index
            .set .Lmov_width\@, ASM_WORD_BITS
        .endif
    .endr
    .if .Lmov_width\@ == ASM_WORD_BITS
        .if ((\val) < ASM_SIGNED_WORD_MIN) || ((\val) > ASM_UNSIGNED_WORD_MAX)
            .error "mov_imm value exceeds signed/unsigned 32-bit range"
        .endif
    .endif
    .set .Lmov_started\@, 0
    .irp shift, ASM_IMMEDIATE_SHIFTS
        .if \shift < .Lmov_width\@
            .if ((\val) >> \shift) & ASM_HALFWORD_MASK
                .if .Lmov_started\@
                    movk \reg, #(((\val) >> \shift) & ASM_HALFWORD_MASK), lsl #\shift
                .else
                    movz \reg, #(((\val) >> \shift) & ASM_HALFWORD_MASK), lsl #\shift
                    .set .Lmov_started\@, 1
                .endif
            .endif
        .endif
    .endr
    .if .Lmov_started\@ == 0
        movz \reg, #0
    .endif
.endm

/* Symbol address macros -------------------------------------------------- */
/* Form a PC-relative symbol address within ADRP's +/-4 GiB range. */
.macro load_addr reg:req, symbol:req
    adrp \reg, \symbol
    add  \reg, \reg, :lo12:\symbol
.endm

/* Exception vector macros ------------------------------------------------ */
/* Place sixteen 128-byte exception slots in a 2 KiB table. */
.macro vector_base name:req
    .set .Lvector_entry_count, 0
    .section .vectors, "ax", %progbits
    .p2align A64_VECTOR_ALIGN_SHIFT, A64_VECTOR_PADDING_BYTE
    .global \name
    .type \name, %object
\name:
.endm

.macro vector_entry name:req
    .set .Lvector_entry_count, .Lvector_entry_count + 1
    .section .vectors, "ax", %progbits
    .p2align A64_VECTOR_SLOT_ALIGN_SHIFT, A64_VECTOR_PADDING_BYTE
    .global \name
    .type \name, %function
\name:
    .cfi_sections .debug_frame
    .cfi_startproc
    unwind_root
.endm

.macro end_vector_entry name:req
    .cfi_endproc
    .size \name, . - \name
    .if (. - \name) > A64_VECTOR_SLOT_SIZE
        .error "exception vector exceeds its 128-byte slot"
    .else
        /* Zero padding encodes undefined instructions, trapping if executed. */
        .fill A64_VECTOR_SLOT_SIZE - (. - \name), ASM_PADDING_UNIT_BYTES, A64_VECTOR_PADDING_BYTE
    .endif
.endm

.macro end_vector_table name:req
    .if .Lvector_entry_count != A64_VECTOR_SLOT_COUNT
        .error "AArch64 exception vector table must contain 16 slots"
    .endif
    /*
     * Alignment fragments prevent a direct .if comparison of the table size. Sixteen
     * bounded slots reach this boundary exactly; extra bytes cause the assembler to
     * reject the backward .org.
     */
    .org \name + A64_VECTOR_TABLE_SIZE, A64_VECTOR_PADDING_BYTE
    .size \name, . - \name
.endm

#endif /* SIMPLE_BOOT_ASM_AARCH64_S */
