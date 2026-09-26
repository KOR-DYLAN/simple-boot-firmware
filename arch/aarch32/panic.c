/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch32/panic.c
 * Brief: Capture and report AArch32 processor state during a panic.
 */

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "arch/panic.h"
#include "driver/console.h"

/* Private helpers -------------------------------------------------------- */
static void print_register(const char *name, uint32_t value)
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
    uint32_t stack_pointer;
    uint32_t frame_pointer;
    uint32_t link_register;
    uint32_t current_status;
    uint32_t control;
    uint32_t vector_base;
    uint32_t data_status;
    uint32_t instruction_status;
    uint32_t data_address;
    uint32_t instruction_address;

    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    __asm__ volatile ("mov %0, r11" : "=r" (frame_pointer));
    __asm__ volatile ("mov %0, lr" : "=r" (link_register));
    __asm__ volatile ("mrs %0, cpsr" : "=r" (current_status));
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (control));
    __asm__ volatile ("mrc p15, 0, %0, c12, c0, 0" : "=r" (vector_base));
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (data_status));
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 1" : "=r" (instruction_status));
    __asm__ volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (data_address));
    __asm__ volatile ("mrc p15, 0, %0, c6, c0, 2" : "=r" (instruction_address));

    print_register("PC", (uint32_t)program_counter);
    print_register("SP", stack_pointer);
    print_register("FP", frame_pointer);
    print_register("LR", link_register);
    print_register("CPSR", current_status);
    print_register("SCTLR", control);
    print_register("VBAR", vector_base);
    print_register("DFSR", data_status);
    print_register("IFSR", instruction_status);
    print_register("DFAR", data_address);
    print_register("IFAR", instruction_address);
}
