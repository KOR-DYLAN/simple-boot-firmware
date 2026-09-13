/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/arch/arch_def.h
 * Brief: Define instruction, ABI, processor-state, and vector constants.
 */

#ifndef SIMPLE_BOOT_ARCH_DEF_H
#define SIMPLE_BOOT_ARCH_DEF_H

/* Instruction and ABI macros --------------------------------------------- */
/* Instruction encoding, ABI alignment, and constant loading. */
#define ARCH_FUNCTION_ALIGN_SHIFT 2
#define ARCH_THUMB_ALIGN_SHIFT    1
#define ARCH_RESET_ALIGN_SHIFT    4
#define ARCH_FRAME_CHAIN_END      0
#define ASM_INITIAL_CFA_OFFSET    0
#define ASM_HALFWORD_BITS         16
#define ASM_HALFWORD_MASK         0xffff
#define ASM_WORD_BITS             32
#define ASM_DWORD_BITS            64
#define ASM_SIGNED_WORD_MIN       (-0x80000000)
#define ASM_UNSIGNED_WORD_MAX     0xffffffff
#define ASM_WORD_UPPER_MASK       0xffff0000
#define ASM_IMMEDIATE_SHIFTS      0, 16, 32, 48
#define ASM_PADDING_UNIT_BYTES    1
#define ASM_BYTE_SIZE             1
#define ASM_ZERO_VALUE            0

/* AArch64 processor macros ----------------------------------------------- */
/* AArch64 reset state and exception vector layout. */
#define A64_CURRENT_EL1              (1 << 2)
#define A64_CURRENT_EL2              (2 << 2)
#define A64_CURRENT_EL3              (3 << 2)
#define A64_HCR_RW                   (1 << 31)
#define A64_CPTR_EL2_RES1            0x33ff
#define A64_CNTHCTL_EL1_PCTEN        (1 << 0)
#define A64_CNTHCTL_EL1_PCEN         (1 << 1)
#define A64_CNTHCTL_EL1_TIMER_ACCESS (A64_CNTHCTL_EL1_PCEN | A64_CNTHCTL_EL1_PCTEN)
#define A64_DAIF_ALL                 0xf
#define A64_SPSR_FIQ_MASK            (1 << 6)
#define A64_SPSR_IRQ_MASK            (1 << 7)
#define A64_SPSR_ABORT_MASK          (1 << 8)
#define A64_SPSR_DEBUG_MASK          (1 << 9)
#define A64_SPSR_EL1H                0x5
#define A64_SPSR_MASKED_EL1H         \
    (A64_SPSR_DEBUG_MASK | A64_SPSR_ABORT_MASK | A64_SPSR_IRQ_MASK | \
     A64_SPSR_FIQ_MASK | A64_SPSR_EL1H)
#define A64_SPSEL_CURRENT_EL        1
#define A64_SPSEL_EL1               A64_SPSEL_CURRENT_EL
#define A64_SCTLR_M                 (1 << 0)
#define A64_SCTLR_A                 (1 << 1)
#define A64_SCTLR_C                 (1 << 2)
#define A64_SCTLR_I                 (1 << 12)
#define A64_SCTLR_E0E               (1 << 24)
#define A64_SCTLR_EE                (1 << 25)
#define A64_VECTOR_ALIGN_SHIFT      11
#define A64_VECTOR_ALIGNMENT        (1 << A64_VECTOR_ALIGN_SHIFT)
#define A64_VECTOR_SLOT_ALIGN_SHIFT 7
#define A64_VECTOR_SLOT_SIZE        (1 << A64_VECTOR_SLOT_ALIGN_SHIFT)
#define A64_VECTOR_SLOT_COUNT       16
#define A64_VECTOR_TABLE_SIZE       (A64_VECTOR_SLOT_COUNT * A64_VECTOR_SLOT_SIZE)
#define A64_VECTOR_PADDING_BYTE     0
#define A64_STACK_ALIGNMENT         16
#define A64_W_REGISTER_SUFFIXES     \
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, \
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, zr, \
    ZR

/* AArch32 processor macros ----------------------------------------------- */
/* AArch32 processor modes, control bits, and CP15 register tuples. */
#define ARM_CPSR_MODE_MASK     0x1f
#define ARM_CPSR_MODE_HYP      0x1a
#define ARM_CPSR_MODE_SVC      0x13
#define ARM_CPSR_MODE_MON      0x16
#define ARM_SCR_NS             (1 << 0)
#define ARM_CPSR_FIQ_MASK      (1 << 6)
#define ARM_CPSR_IRQ_MASK      (1 << 7)
#define ARM_CPSR_ABORT_MASK    (1 << 8)
#define ARM_CPSR_AIF           (ARM_CPSR_ABORT_MASK | ARM_CPSR_IRQ_MASK | ARM_CPSR_FIQ_MASK)
#define ARM_SVC_RESET_PSR      (ARM_CPSR_AIF | ARM_CPSR_MODE_SVC)
#define ARM_HCPTR_RES1         0x33ff
#define ARM_HYP_TRAPS_DISABLED 0
#define ARM_SCTLR_M            (1 << 0)
#define ARM_SCTLR_A            (1 << 1)
#define ARM_SCTLR_C            (1 << 2)
#define ARM_SCTLR_I            (1 << 12)
#define ARM_SCTLR_V            (1 << 13)
#define ARM_SCTLR_EE           (1 << 25)
#define ARM_HVBAR              p15, 4, c12, c0, 0
#define ARM_HCR                p15, 4, c1, c1, 0
#define ARM_HSTR               p15, 4, c1, c1, 3
#define ARM_HCPTR              p15, 4, c1, c1, 2
#define ARM_SCTLR              p15, 0, c1, c0, 0
#define ARM_VBAR               p15, 0, c12, c0, 0
#define ARM_MVBAR              p15, 0, c12, c0, 1
#define ARM_SCR                p15, 0, c1, c1, 0
#define ARM_VECTOR_ALIGNMENT   32
#define ARM_VECTOR_SLOT_SIZE   4
#define ARM_VECTOR_SLOT_COUNT  8
#define ARM_VECTOR_TABLE_SIZE  (ARM_VECTOR_SLOT_COUNT * ARM_VECTOR_SLOT_SIZE)
#define ARM_STACK_ALIGNMENT    8

/* Cortex-M processor macros ---------------------------------------------- */
/* ARMv7-M thread mode and core exception vectors. */
#define CORTEX_M_SCB_VTOR               0xe000ed08
#define CORTEX_M_CONTROL_PRIVILEGED_MSP 0
#define CORTEX_M_CORE_VECTOR_COUNT      16
#define CORTEX_M_MIN_VECTOR_ALIGNMENT   128
#define CORTEX_M_VECTOR_ENTRY_SIZE      4
#define CORTEX_M_RESERVED_VECTOR        0
#define CORTEX_M_RESERVED_CORE_SLOTS    4
#define CORTEX_M_STACK_ALIGNMENT        8

#if defined(BOOT_ARCH_AARCH64)
#define ARCH_STACK_ALIGNMENT  A64_STACK_ALIGNMENT
#define ARCH_VECTOR_ALIGNMENT A64_VECTOR_ALIGNMENT
#elif defined(BOOT_ARCH_AARCH32)
#define ARCH_STACK_ALIGNMENT  ARM_STACK_ALIGNMENT
#define ARCH_VECTOR_ALIGNMENT ARM_VECTOR_ALIGNMENT
#elif defined(BOOT_ARCH_CORTEX_M)
#define ARCH_STACK_ALIGNMENT  CORTEX_M_STACK_ALIGNMENT
#define ARCH_VECTOR_ALIGNMENT CORTEX_M_MIN_VECTOR_ALIGNMENT
#endif

#endif /* SIMPLE_BOOT_ARCH_DEF_H */
