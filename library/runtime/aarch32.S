/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/runtime/aarch32.S
 * Brief: Initialize ARM and Thumb-2 memory regions without stack access.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.syntax unified
#if defined(BOOT_ARCH_CORTEX_M)
.thumb
#else
.arm
#endif

/* Boot-only runtime entry ------------------------------------------------ */
/* Helpers preserve r12. No stack frames may be live while this function runs. */
func runtime_init
    mov     r12, lr
    .cfi_register lr, r12
    bl      boot_copy_data
    bl      boot_clear_bss
    bl      boot_clear_stack
    bx      r12
endfunc runtime_init

/* Initialized data relocation -------------------------------------------- */
func boot_copy_data
    load_addr r0, __data_load_start
    load_addr r1, __data_start
    load_addr r2, __data_end
.Lcopy:
    cmp     r1, r2
    bhs     .Lcopy_done
    ldrb    r3, [r0], #ASM_BYTE_SIZE
    strb    r3, [r1], #ASM_BYTE_SIZE
    b       .Lcopy
.Lcopy_done:
    bx      lr
endfunc boot_copy_data

/* Zero-initialized regions ----------------------------------------------- */
func boot_clear_bss
    load_addr r0, __bss_start
    load_addr r1, __bss_end
    b       boot_zero_range
endfunc boot_clear_bss

func boot_clear_stack
    load_addr r0, __stack_bottom
    load_addr r1, __stack_top
    b       boot_zero_range
endfunc boot_clear_stack

/* Leaf helper: clear [r0, r1), including unaligned boundaries. */
func boot_zero_range, linkage=local
    movs    r2, #ASM_ZERO_VALUE
.Lzero:
    cmp     r0, r1
    bhs     .Lzero_done
    strb    r2, [r0], #ASM_BYTE_SIZE
    b       .Lzero
.Lzero_done:
    bx      lr
endfunc boot_zero_range

.section .note.GNU-stack, "", %progbits
