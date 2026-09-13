/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch32/startup.S
 * Brief: Initialize AArch32 execution state, vectors, stack, and runtime.
 */

/*
 * Initialize little-endian ARM state. Entry must be a cold reset in
 * SVC, HYP, or Monitor with the MMU and caches disabled. Secure-entry builds
 * select Monitor mode; other builds select SVC. Memory initialization uses no stack.
 */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.syntax unified
.arm

/* Reset entry ------------------------------------------------------------ */
func _start, section=.text.boot
    unwind_root
    cpsid   aif
#if defined(BOOT_AARCH32_SECURE_ENTRY)
    /* Secure reset can enter in SVC; select the security monitor explicitly. */
    cps     #ARM_CPSR_MODE_MON
    ldcopr  r0, ARM_SCR
    bic     r0, r0, #ARM_SCR_NS
    stcopr  r0, ARM_SCR
    load_addr r0, aarch32_vectors
    stcopr  r0, ARM_MVBAR
    b       .Lprivileged_entry
#else
    mrs     r0, cpsr
    and     r0, r0, #ARM_CPSR_MODE_MASK
    cmp     r0, #ARM_CPSR_MODE_HYP
    bne     .Lenter_svc

    /* Leave HYP through an exception return. */
    load_addr r0, aarch32_vectors
    stcopr  r0, ARM_HVBAR
    mov     r0, #ARM_HYP_TRAPS_DISABLED
    stcopr  r0, ARM_HCR            /* Disable stage 2 translation and traps. */
    stcopr  r0, ARM_HSTR
    mov_imm r0, ARM_HCPTR_RES1     /* Preserve RES1 fields. */
    stcopr  r0, ARM_HCPTR          /* Disable coprocessor traps. */
    load_addr r0, .Lsvc_entry
    msr     elr_hyp, r0
    mov_imm r0, ARM_SVC_RESET_PSR  /* Masked interrupts, little-endian ARM state. */
    msr     spsr_cxsf, r0
    isb
    eret

.Lenter_svc:
    cps     #ARM_CPSR_MODE_SVC
.Lsvc_entry:
#endif
.Lprivileged_entry:
    setend  le
    /*
     * Preserve RES1 bits; disable the MMU, alignment checks, and caches. Select VBAR
     * instead of high vectors.
     */
    ldcopr  r0, ARM_SCTLR
    bic     r0, r0, #(ARM_SCTLR_M | ARM_SCTLR_A | ARM_SCTLR_C)
    bic     r0, r0, #ARM_SCTLR_I
    bic     r0, r0, #ARM_SCTLR_V
    bic     r0, r0, #ARM_SCTLR_EE
    stcopr  r0, ARM_SCTLR
    load_addr r0, aarch32_vectors
    stcopr  r0, ARM_VBAR
    isb

    bl      runtime_init
    load_addr r0, __stack_top
    mov     sp, r0
    mov     r11, #ARCH_FRAME_CHAIN_END
    bl      boot_main
    b       boot_halt
endfunc _start

/* Halt entry ------------------------------------------------------------- */
func boot_halt
    unwind_root
    cpsid   aif
.Lhalt:
    wfi
    b       .Lhalt
endfunc boot_halt

/* Default exception handler ---------------------------------------------- */
func default_exception_handler
    unwind_root
    /* Inspect CPSR, SPSR, banked LR or ELR_hyp, and DFSR/IFSR/DFAR/IFAR in GDB. */
    cpsid   aif
.Lexception:
    wfi
    b       .Lexception
endfunc default_exception_handler

/* Exception vector table ------------------------------------------------- */
.section .vectors, "ax", %progbits
.balign ARM_VECTOR_ALIGNMENT
.global aarch32_vectors
.type aarch32_vectors, %object
aarch32_vectors:
    b       _start
    b       default_exception_handler  /* Undefined instruction */
    b       default_exception_handler  /* SVC */
    b       default_exception_handler  /* Prefetch abort */
    b       default_exception_handler  /* Data abort */
    b       default_exception_handler  /* Reserved / HYP trap */
    b       default_exception_handler  /* IRQ */
    b       default_exception_handler  /* FIQ */
.if (. - aarch32_vectors) != ARM_VECTOR_TABLE_SIZE
    .error "AArch32 requires eight 4-byte vector slots"
.endif
.size aarch32_vectors, . - aarch32_vectors

.section .note.GNU-stack, "", %progbits
