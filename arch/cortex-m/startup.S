/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/cortex-m/startup.S
 * Brief: Define ARMv7-M reset, vectors, and default exception handling.
 */

/* ARMv7-M reset and exception vectors. */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"
#include "platform_def.h"

/* Assembler configuration ------------------------------------------------ */
.syntax unified
.thumb

/* Exception vector table ------------------------------------------------- */
.section .vectors, "a", %progbits
/* The platform supplies the external IRQ count and vector alignment. */
.if PLATFORM_VECTOR_ALIGNMENT < CORTEX_M_MIN_VECTOR_ALIGNMENT
    .error "Vector alignment is below the architectural minimum"
.endif
.if PLATFORM_VECTOR_ALIGNMENT < ((CORTEX_M_CORE_VECTOR_COUNT + PLATFORM_IRQ_COUNT) * CORTEX_M_VECTOR_ENTRY_SIZE)
    .error "Vector alignment must cover the complete table"
.endif
.balign PLATFORM_VECTOR_ALIGNMENT
.global vector_table
.type vector_table, %object
vector_table:
    .word __stack_top
    .word Reset_Handler
    vector_handler NMI_Handler
    vector_handler HardFault_Handler
    vector_handler MemManage_Handler
    vector_handler BusFault_Handler
    vector_handler UsageFault_Handler
    .fill CORTEX_M_RESERVED_CORE_SLOTS, CORTEX_M_VECTOR_ENTRY_SIZE, CORTEX_M_RESERVED_VECTOR
    vector_handler SVC_Handler
    vector_handler DebugMon_Handler
    .word CORTEX_M_RESERVED_VECTOR
    vector_handler PendSV_Handler
    vector_handler SysTick_Handler
    .irp irq, PLATFORM_IRQ_NUMBERS
    vector_handler IRQ\irq\()_Handler
    .endr
.if (. - vector_table) != ((CORTEX_M_CORE_VECTOR_COUNT + PLATFORM_IRQ_COUNT) * CORTEX_M_VECTOR_ENTRY_SIZE)
    .error "Vector count does not match the platform IRQ configuration"
.endif
.size vector_table, . - vector_table

/* Reset entry ------------------------------------------------------------ */
func Reset_Handler, section=.text.boot
    unwind_root
    cpsid   i
    movs    r0, #CORTEX_M_CONTROL_PRIVILEGED_MSP
    msr     control, r0
    isb

    /* Point SCB VTOR to the vector table placed by the linker. */
    mov_imm r0, CORTEX_M_SCB_VTOR
    load_addr r1, vector_table
    str     r1, [r0]
    dsb
    isb

    /* No live stack frames exist while the assembly runtime clears the stack. */
    bl      runtime_init
    load_addr r0, __stack_top
    msr     msp, r0
    bl      boot_main
    b       boot_halt
endfunc Reset_Handler

func_alias _start, Reset_Handler

/* Default exception handler ---------------------------------------------- */
func default_exception_handler
    unwind_root
    /* Preserve the stacked exception frame and IPSR for GDB. */
    cpsid   i
.Lexception_wait:
    b       .Lexception_wait
endfunc default_exception_handler

/* Halt entry ------------------------------------------------------------- */
func boot_halt
    unwind_root
    cpsid   i
.Lhalt_wait:
    wfi
    b       .Lhalt_wait
endfunc boot_halt

.section .note.GNU-stack, "", %progbits
