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
    mov     ip, r0
    mov     r0, r1
    mov     r1, r2
    mov     r2, r3
    ldr     r3, [sp]
    dsb     sy
    isb
    bx      ip
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
