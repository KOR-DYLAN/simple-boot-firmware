/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch64/panic.c
 * Brief: Capture and report AArch64 processor state during a panic.
 */

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "arch/arch_def.h"
#include "arch/panic.h"
#include "driver/console.h"

/* Private helpers -------------------------------------------------------- */
static void print_register(const char *name, uint64_t value)
{
    console_puts("  ");
    console_puts(name);
    console_puts(" = ");
    console_hex((uintptr_t)value);
    console_puts("\n");
}

/* Panic diagnostics ------------------------------------------------------ */
void arch_panic_dump(uintptr_t program_counter)
{
    uint64_t stack_pointer;
    uint64_t frame_pointer;
    uint64_t link_register;
    uint64_t current_el;
    uint64_t interrupt_mask;
    uint64_t control;
    uint64_t vector_base;
    uint64_t syndrome;
    uint64_t exception_link;
    uint64_t saved_status;

    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    __asm__ volatile ("mov %0, x29" : "=r" (frame_pointer));
    __asm__ volatile ("mov %0, x30" : "=r" (link_register));
    __asm__ volatile ("mrs %0, CurrentEL" : "=r" (current_el));
    __asm__ volatile ("mrs %0, DAIF" : "=r" (interrupt_mask));

    control = ULL(0);
    vector_base = ULL(0);
    syndrome = ULL(0);
    exception_link = ULL(0);
    saved_status = ULL(0);
    if ((current_el & A64_CURRENT_EL_MASK) == A64_CURRENT_EL3) {
        __asm__ volatile ("mrs %0, sctlr_el3" : "=r" (control));
        __asm__ volatile ("mrs %0, vbar_el3" : "=r" (vector_base));
        __asm__ volatile ("mrs %0, esr_el3" : "=r" (syndrome));
        __asm__ volatile ("mrs %0, elr_el3" : "=r" (exception_link));
        __asm__ volatile ("mrs %0, spsr_el3" : "=r" (saved_status));
    } else if ((current_el & A64_CURRENT_EL_MASK) == A64_CURRENT_EL2) {
        __asm__ volatile ("mrs %0, sctlr_el2" : "=r" (control));
        __asm__ volatile ("mrs %0, vbar_el2" : "=r" (vector_base));
        __asm__ volatile ("mrs %0, esr_el2" : "=r" (syndrome));
        __asm__ volatile ("mrs %0, elr_el2" : "=r" (exception_link));
        __asm__ volatile ("mrs %0, spsr_el2" : "=r" (saved_status));
    } else {
        __asm__ volatile ("mrs %0, sctlr_el1" : "=r" (control));
        __asm__ volatile ("mrs %0, vbar_el1" : "=r" (vector_base));
        __asm__ volatile ("mrs %0, esr_el1" : "=r" (syndrome));
        __asm__ volatile ("mrs %0, elr_el1" : "=r" (exception_link));
        __asm__ volatile ("mrs %0, spsr_el1" : "=r" (saved_status));
    }

    print_register("PC", program_counter);
    print_register("SP", stack_pointer);
    print_register("FP", frame_pointer);
    print_register("LR", link_register);
    print_register("CurrentEL", current_el);
    print_register("DAIF", interrupt_mask);
    print_register("SCTLR_ELx", control);
    print_register("VBAR_ELx", vector_base);
    print_register("ESR_ELx", syndrome);
    print_register("ELR_ELx", exception_link);
    print_register("SPSR_ELx", saved_status);
}
