/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/runtime/aarch64.S
 * Brief: Initialize data, BSS, and stack without using the stack pointer.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.arch armv8-a

/* Boot-only runtime entry ------------------------------------------------ */
/* Helpers preserve x9. No stack frames may be live while this function runs. */
func runtime_init
    mov     x9, x30
    .cfi_register x30, x9
    bl      boot_copy_data
    bl      boot_clear_bss
    bl      boot_clear_stack
    ret     x9
endfunc runtime_init

/* Initialized data relocation -------------------------------------------- */
func boot_copy_data
    load_addr x0, __data_load_start
    load_addr x1, __data_start
    load_addr x2, __data_end
.Lcopy:
    cmp     x1, x2
    b.hs    .Lcopy_done
    ldrb    w3, [x0], #ASM_BYTE_SIZE
    strb    w3, [x1], #ASM_BYTE_SIZE
    b       .Lcopy
.Lcopy_done:
    ret
endfunc boot_copy_data

/* Zero-initialized regions ----------------------------------------------- */
func boot_clear_bss
    load_addr x0, __bss_start
    load_addr x1, __bss_end
    b       boot_zero_range
endfunc boot_clear_bss

func boot_clear_stack
    load_addr x0, __stack_bottom
    load_addr x1, __stack_top
    b       boot_zero_range
endfunc boot_clear_stack

/* Leaf helper: clear [x0, x1), including unaligned boundaries. */
func boot_zero_range, linkage=local
.Lzero:
    cmp     x0, x1
    b.hs    .Lzero_done
    strb    wzr, [x0], #ASM_BYTE_SIZE
    b       .Lzero
.Lzero_done:
    ret
endfunc boot_zero_range

.section .note.GNU-stack, "", %progbits
