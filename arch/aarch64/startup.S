/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch64/startup.S
 * Brief: Initialize AArch64 execution state, stack, and runtime.
 */

/*
 * Initialize little-endian AArch64 state. Entry must be a cold reset at
 * EL1, EL2, or EL3 with the MMU and caches disabled. EL3 entry retains EL3h;
 * EL2 entry descends to EL1h. Memory is initialized before the first C call.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.arch armv8-a

/* Reset entry ------------------------------------------------------------ */
func _start, align=ARCH_RESET_ALIGN_SHIFT, section=.text.boot
    unwind_root
    msr     daifset, #A64_DAIF_ALL
    mrs     x0, CurrentEL
    cmp     x0, #A64_CURRENT_EL3
    b.eq    .Lel3_entry
    cmp     x0, #A64_CURRENT_EL2
    b.eq    .Lfrom_el2
    cmp     x0, #A64_CURRENT_EL1
    b.ne    default_exception_handler
    b       .Lel1_entry

.Lel3_entry:
    msr     spsel, #A64_SPSEL_CURRENT_EL
    mrs     x0, sctlr_el3
    bic     x0, x0, #(A64_SCTLR_M | A64_SCTLR_A | A64_SCTLR_C)
    bic     x0, x0, #A64_SCTLR_I
    bic     x0, x0, #A64_SCTLR_EE
    msr     sctlr_el3, x0
    msr     cptr_el3, xzr
    load_addr x0, aarch64_vectors
    msr     vbar_el3, x0
    isb
    b       .Lruntime_entry

.Lfrom_el2:
    load_addr x0, aarch64_vectors
    msr     vbar_el2, x0

    /* Select AArch64 EL1; disable stage 2 translation and EL2 traps/routing. */
    mov_imm x0, A64_HCR_RW
    msr     hcr_el2, x0
    mov_imm x0, A64_CPTR_EL2_RES1
    msr     cptr_el2, x0
    msr     hstr_el2, xzr
    mov_imm x0, A64_CNTHCTL_EL1_TIMER_ACCESS
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    /* Mask D/A/I/F and select little-endian EL1h with SP_EL1. */
    mov_imm x0, A64_SPSR_MASKED_EL1H
    msr     spsr_el2, x0
    adr     x0, .Lel1_entry
    msr     elr_el2, x0
    isb
    eret

.Lel1_entry:
    msr     spsel, #A64_SPSEL_EL1
    /* Preserve RES1 bits; disable the MMU, alignment checks, and caches. */
    mrs     x0, sctlr_el1
    bic     x0, x0, #(A64_SCTLR_M | A64_SCTLR_A | A64_SCTLR_C)
    bic     x0, x0, #A64_SCTLR_I
    bic     x0, x0, #(A64_SCTLR_E0E | A64_SCTLR_EE)
    msr     sctlr_el1, x0
    load_addr x0, aarch64_vectors
    msr     vbar_el1, x0
    isb

.Lruntime_entry:
    /* The assembly runtime does not read or write through SP. */
    bl      runtime_init
    load_addr x0, __stack_top
    mov     sp, x0
    mov     x29, xzr
    mov     x30, xzr
    bl      boot_main
    b       boot_halt
endfunc _start

/* Halt entry ------------------------------------------------------------- */
func boot_halt
    unwind_root
    msr     daifset, #A64_DAIF_ALL
.Lhalt:
    wfi
    b       .Lhalt
endfunc boot_halt

.section .note.GNU-stack, "", %progbits
