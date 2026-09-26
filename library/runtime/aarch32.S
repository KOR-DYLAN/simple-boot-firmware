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
/*
 * Save LR in r12 because stack memory is about to be cleared. Every called
 * helper preserves r12, so runtime_init can return without creating a frame.
 */
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
    /* Copy the linker-defined .data load image into its writable VMA. */
    load_addr r0, __data_load_start
    load_addr r1, __data_start
    load_addr r2, __data_end
    /*
     * Pseudocode:
     * while (destination < destination_end) {
     *     *destination++ = *source++;
     * }
     */
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
    /* C requires every byte in [__bss_start, __bss_end) to begin as zero. */
    load_addr r0, __bss_start
    load_addr r1, __bss_end
    b       boot_zero_range
endfunc boot_clear_bss

func boot_clear_stack
    /* Remove previous image state from the complete reserved stack range. */
    load_addr r0, __stack_bottom
    load_addr r1, __stack_top
    b       boot_zero_range
endfunc boot_clear_stack

/*
 * Clear the half-open range [r0, r1) one byte at a time. Byte stores support
 * linker boundaries that are not naturally aligned and need no stack access.
 */
func boot_zero_range, linkage=local
    movs    r2, #ASM_ZERO_VALUE
    /*
     * Pseudocode:
     * while (start < end) {
     *     *start++ = 0;
     * }
     */
.Lzero:
    cmp     r0, r1
    bhs     .Lzero_done
    strb    r2, [r0], #ASM_BYTE_SIZE
    b       .Lzero
.Lzero_done:
    bx      lr
endfunc boot_zero_range

.section .note.GNU-stack, "", %progbits
