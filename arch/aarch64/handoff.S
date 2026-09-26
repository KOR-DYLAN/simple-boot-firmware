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
    /* Preserve entry in x5 while shifting four payload arguments into x0-x3. */
    mov     x5, x0
    mov     x0, x1
    mov     x1, x2
    mov     x2, x3
    mov     x3, x4

    /* Complete prior memory accesses and context changes before new code runs. */
    dsb     sy
    isb

    /* Branch without a link so the destination cannot return to this image. */
    br      x5
endfunc boot_jump

.section .note.GNU-stack, "", %progbits
