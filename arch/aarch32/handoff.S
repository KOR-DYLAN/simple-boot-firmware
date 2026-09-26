/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch32/handoff.S
 * Brief: Transfer execution to the next AArch32 boot image.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.syntax unified
.arm

/* Image handoff ---------------------------------------------------------- */
func boot_jump
    /*
     * Preserve entry in ip while shifting payload arguments into r0-r3. The
     * fifth AAPCS argument is read from the caller's stack before SP ownership
     * transfers to the destination image.
     */
    mov     ip, r0
    mov     r0, r1
    mov     r1, r2
    mov     r2, r3
    ldr     r3, [sp]

    /* Complete prior memory accesses and context changes before new code runs. */
    dsb     sy
    isb

    /* BX accepts either ARM or Thumb entry addresses and does not set LR. */
    bx      ip
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
