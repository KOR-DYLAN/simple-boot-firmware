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
    /*
     * Mask debug, SError, IRQ, and FIQ before installing a vector base. An
     * exception taken during this window would otherwise use unknown firmware
     * state. CurrentEL then selects the register bank that this image owns.
     */
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
    /* Use SP_EL3 so later stack installation updates the current EL stack. */
    msr     spsel, #A64_SPSEL_CURRENT_EL
    /*
     * Keep implementation-defined and RES1 SCTLR_EL3 bits unchanged. Clear M,
     * A, C, and I to run with translation, alignment checks, and both caches
     * disabled. Clear EE so EL3 data accesses use little-endian byte order.
     */
    mrs     x0, sctlr_el3
    bic     x0, x0, #(A64_SCTLR_M | A64_SCTLR_A | A64_SCTLR_C)
    bic     x0, x0, #A64_SCTLR_I
    bic     x0, x0, #A64_SCTLR_EE
    msr     sctlr_el3, x0

    /* Clear CPTR_EL3 trap controls so lower-level FP/SIMD access is not trapped. */
    msr     cptr_el3, xzr

    /* Route all EL3 exceptions to this image before enabling stack-backed code. */
    load_addr x0, aarch64_vectors
    msr     vbar_el3, x0
    isb
    b       .Lruntime_entry

.Lfrom_el2:
    /* Catch any exception raised while EL2 prepares the EL1 transition. */
    load_addr x0, aarch64_vectors
    msr     vbar_el2, x0

    /*
     * Set HCR_EL2.RW so EL1 executes AArch64. Leaving VM and routing/trap bits
     * clear disables stage-2 translation and keeps exceptions at their normal
     * destination instead of redirecting them to EL2.
     */
    mov_imm x0, A64_HCR_RW
    msr     hcr_el2, x0

    /* Preserve CPTR_EL2 RES1 bits while leaving coprocessor trap bits clear. */
    mov_imm x0, A64_CPTR_EL2_RES1
    msr     cptr_el2, x0

    /* Do not trap AArch32 system-register accesses from a lower level. */
    msr     hstr_el2, xzr

    /*
     * Permit EL1 physical counter and timer access. A zero virtual offset keeps
     * virtual counter time aligned with the physical counter for early boot.
     */
    mov_imm x0, A64_CNTHCTL_EL1_TIMER_ACCESS
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    /*
     * Program the exception-return state for EL1h: use SP_EL1 and mask debug,
     * SError, IRQ, and FIQ. ELR_EL2 supplies the first EL1 instruction.
     */
    mov_imm x0, A64_SPSR_MASKED_EL1H
    msr     spsr_el2, x0
    adr     x0, .Lel1_entry
    msr     elr_el2, x0
    isb
    eret

.Lel1_entry:
    /* Select SP_EL1 instead of the shared SP_EL0 bank. */
    msr     spsel, #A64_SPSEL_EL1
    /*
     * Preserve unrelated SCTLR_EL1 state while clearing M, A, C, and I. This
     * establishes flat physical addressing with alignment checks and caches
     * disabled. E0E and EE select little-endian accesses at EL0 and EL1.
     */
    mrs     x0, sctlr_el1
    bic     x0, x0, #(A64_SCTLR_M | A64_SCTLR_A | A64_SCTLR_C)
    bic     x0, x0, #A64_SCTLR_I
    bic     x0, x0, #(A64_SCTLR_E0E | A64_SCTLR_EE)
    msr     sctlr_el1, x0

    /* Install the EL1 exception table before entering the common runtime. */
    load_addr x0, aarch64_vectors
    msr     vbar_el1, x0
    isb

.Lruntime_entry:
    /*
     * runtime_init relocates .data and clears .bss and the stack without using
     * SP. Install the linker-defined stack only after its storage is cleared.
     */
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
    /* Mask every asynchronous exception so WFI cannot resume normal boot flow. */
    msr     daifset, #A64_DAIF_ALL
    /*
     * Pseudocode:
     * for (;;) {
     *     wait_for_interrupt();
     * }
     */
.Lhalt:
    wfi
    b       .Lhalt
endfunc boot_halt

.section .note.GNU-stack, "", %progbits
