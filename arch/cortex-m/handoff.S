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
    /*
     * The entry argument names a Cortex-M vector table, not executable code.
     * Stop configurable interrupts before replacing VTOR and the active stack.
     */
    cpsid   i

    /* Vector slot zero supplies MSP; slot one supplies the Thumb reset handler. */
    ldr     r2, [r0]
    ldr     r3, [r0, #CORTEX_M_VECTOR_ENTRY_SIZE]

    /*
     * Publish the destination vector table. DSB completes the VTOR write and
     * ISB discards stale exception context before MSP and control are replaced.
     */
    mov_imm r1, CORTEX_M_SCB_VTOR
    str     r0, [r1]
    dsb     sy
    isb

    /* Enter the destination with its initial stack and reset-handler address. */
    msr     msp, r2
    bx      r3
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
