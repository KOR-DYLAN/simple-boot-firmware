/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/cortex-m/handoff.S
 * Brief: Transfer execution through an ARMv7-M vector table.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.syntax unified
.thumb

/* Image handoff ---------------------------------------------------------- */
func boot_jump
    cpsid   i
    ldr     r2, [r0]
    ldr     r3, [r0, #CORTEX_M_VECTOR_ENTRY_SIZE]
    mov_imm r1, CORTEX_M_SCB_VTOR
    str     r0, [r1]
    dsb     sy
    isb
    msr     msp, r2
    bx      r3
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
