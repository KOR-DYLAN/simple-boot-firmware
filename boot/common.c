/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/common.c
 * Brief: Validate runtime state shared by both bootloader stages.
 */

/* Includes --------------------------------------------------------------- */
#include "arch/arch_def.h"
#include "boot.h"
#include "boot_status.h"
#include "driver/console.h"
#include "memory_layout.h"

/* Boot state ------------------------------------------------------------- */
volatile uint32_t boot_data_cookie = BOOT_DATA_COOKIE_INITIAL;
volatile uint32_t boot_bss_cookie;
volatile uint32_t boot_status;

/* Private helpers -------------------------------------------------------- */
static void print_region(const char *name, uintptr_t start, uintptr_t end)
{
    console_puts(name);
    console_hex(start);
    console_puts(" .. ");
    console_hex(end);
    console_puts("\n");
}

/* Shared stage operations ------------------------------------------------ */
void boot_print_handoff(const char *name, uintptr_t entry)
{
    console_puts(name);
    console_hex(entry);
    console_puts("\n");
}

/* A stable GDB breakpoint after the runtime checks complete. */
__attribute__((noinline)) void boot_ready(void)
{
    __asm__ volatile ("" ::: "memory");
}

void boot_stage_run(const char *stage_name)
{
    uintptr_t stack_pointer;

#if defined(BOOT_ARCH_AARCH64)
    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    const char *arch = "aarch64";
#elif defined(BOOT_ARCH_AARCH32)
    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    const char *arch = "aarch32";
#else
    __asm__ volatile ("mrs %0, msp" : "=r" (stack_pointer));
    const char *arch = "cortex-m";
#endif

    console_init();
    console_puts("simple-boot ");
    console_puts(stage_name);
    console_puts(" [");
    console_puts(arch);
    console_puts("]\n");
    print_region("CODE     ", CODE_START, CODE_END);
    print_region("RO DATA  ", RO_DATA_START, RO_DATA_END);
    print_region("RESERVED ", RO_DATA_END, RW_DATA_START);
    print_region("RW DATA  ", RW_DATA_START, RW_DATA_END);
    print_region("RESERVED ", RW_DATA_END, STACK_END);
    print_region("STACK    ", STACK_END, STACK_START);
    print_region(".data LMA/VMA ",
                 (uintptr_t)__data_load_start, (uintptr_t)__data_start);

    if (boot_data_cookie != BOOT_DATA_COOKIE_INITIAL ||
        boot_bss_cookie != BOOT_BSS_INITIAL ||
        stack_pointer < STACK_END ||
        stack_pointer >= STACK_START ||
        (stack_pointer & (ARCH_STACK_ALIGNMENT - 1)) != 0) {
        boot_status = BOOT_STATUS_FAILED;
        console_puts("BOOT FAIL: data/bss/stack check\n");
        boot_halt();
    }

    boot_status = BOOT_STATUS_READY;
    boot_ready();
    console_puts("BOOT OK ");
    console_puts(stage_name);
    console_puts(" [");
    console_puts(arch);
    console_puts("]\n");
}
