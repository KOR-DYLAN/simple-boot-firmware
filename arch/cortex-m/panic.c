/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/cortex-m/panic.c
 * Brief: Capture and report ARMv7-M processor state during a panic.
 */

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "arch/panic.h"
#include "compiler_rt.h"
#include "driver/console.h"

/* System control block registers ---------------------------------------- */
#define SCB_ICSR_ADDRESS  U(0xe000ed04)
#define SCB_SHCSR_ADDRESS U(0xe000ed24)
#define SCB_CFSR_ADDRESS  U(0xe000ed28)
#define SCB_HFSR_ADDRESS  U(0xe000ed2c)
#define SCB_DFSR_ADDRESS  U(0xe000ed30)
#define SCB_MMFAR_ADDRESS U(0xe000ed34)
#define SCB_BFAR_ADDRESS  U(0xe000ed38)

/* Private helpers -------------------------------------------------------- */
static uint32_t read_register(uintptr_t address)
{
    const volatile uint32_t *reg;
    uint32_t value;

    reg = (const volatile uint32_t *)address;
    value = *reg;
    return value;
}

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
    uint32_t main_stack;
    uint32_t process_stack;
    uint32_t control;
    uint32_t interrupt_state;
    uint32_t priority_mask;
    uint32_t fault_mask;
    uint32_t exception_number;

    __asm__ volatile ("mrs %0, msp" : "=r" (main_stack));
    __asm__ volatile ("mrs %0, psp" : "=r" (process_stack));
    __asm__ volatile ("mrs %0, control" : "=r" (control));
    __asm__ volatile ("mrs %0, primask" : "=r" (interrupt_state));
    __asm__ volatile ("mrs %0, basepri" : "=r" (priority_mask));
    __asm__ volatile ("mrs %0, faultmask" : "=r" (fault_mask));
    __asm__ volatile ("mrs %0, ipsr" : "=r" (exception_number));

    print_register("PC", (uint32_t)program_counter);
    print_register("MSP", main_stack);
    print_register("PSP", process_stack);
    print_register("CONTROL", control);
    print_register("PRIMASK", interrupt_state);
    print_register("BASEPRI", priority_mask);
    print_register("FAULTMASK", fault_mask);
    print_register("IPSR", exception_number);
    print_register("ICSR", read_register(SCB_ICSR_ADDRESS));
    print_register("SHCSR", read_register(SCB_SHCSR_ADDRESS));
    print_register("CFSR", read_register(SCB_CFSR_ADDRESS));
    print_register("HFSR", read_register(SCB_HFSR_ADDRESS));
    print_register("DFSR", read_register(SCB_DFSR_ADDRESS));
    print_register("MMFAR", read_register(SCB_MMFAR_ADDRESS));
    print_register("BFAR", read_register(SCB_BFAR_ADDRESS));
}
