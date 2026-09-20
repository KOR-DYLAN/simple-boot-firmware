/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch64/handoff.S
 * Brief: Transfer execution to the next AArch64 boot image.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.arch armv8-a

/* Image handoff ---------------------------------------------------------- */
func boot_jump
    mov     x5, x0
    mov     x0, x1
    mov     x1, x2
    mov     x2, x3
    mov     x3, x4
    dsb     sy
    isb
    br      x5
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
