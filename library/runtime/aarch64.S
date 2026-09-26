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
/*
 * Save LR in x9 because stack memory is about to be cleared. Every called
 * helper preserves x9, so runtime_init can return without creating a frame.
 */
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
    /* Copy the linker-defined .data load image into its writable VMA. */
    load_addr x0, __data_load_start
    load_addr x1, __data_start
    load_addr x2, __data_end
    /*
     * Pseudocode:
     * while (destination < destination_end) {
     *     *destination++ = *source++;
     * }
     */
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
    /* C requires every byte in [__bss_start, __bss_end) to begin as zero. */
    load_addr x0, __bss_start
    load_addr x1, __bss_end
    b       boot_zero_range
endfunc boot_clear_bss

func boot_clear_stack
    /* Remove previous image state from the complete reserved stack range. */
    load_addr x0, __stack_bottom
    load_addr x1, __stack_top
    b       boot_zero_range
endfunc boot_clear_stack

/*
 * Clear the half-open range [x0, x1) one byte at a time. Byte stores support
 * linker boundaries that are not naturally aligned and need no stack access.
 */
func boot_zero_range, linkage=local
    /*
     * Pseudocode:
     * while (start < end) {
     *     *start++ = 0;
     * }
     */
.Lzero:
    cmp     x0, x1
    b.hs    .Lzero_done
    strb    wzr, [x0], #ASM_BYTE_SIZE
    b       .Lzero
.Lzero_done:
    ret
endfunc boot_zero_range

.section .note.GNU-stack, "", %progbits
