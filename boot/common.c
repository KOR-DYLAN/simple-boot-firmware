/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/common.c
 * Brief: Validate the boot image runtime state.
 */

/* Includes --------------------------------------------------------------- */
#include <assert.h>
#include <stdio.h>
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

/* Shared boot operations ------------------------------------------------- */
void boot_print_handoff(const char *name, uintptr_t entry)
{
    console_puts(name);
    console_hex(entry);
    console_puts("\n");
}

/* A stable GDB breakpoint after the runtime checks complete. */
COMPILER_NOINLINE void boot_ready(void)
{
    COMPILER_BARRIER();
}

void boot_run(const char *image_name)
{
    uintptr_t stack_pointer;
    const char *arch;

#if defined(BOOT_ARCH_AARCH64)
    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    arch = "aarch64";
#elif defined(BOOT_ARCH_AARCH32)
    __asm__ volatile ("mov %0, sp" : "=r" (stack_pointer));
    arch = "aarch32";
#else
    __asm__ volatile ("mrs %0, msp" : "=r" (stack_pointer));
    arch = "cortex-m";
#endif

    console_init();
    printf("simple-boot %s [%s]\n", image_name, arch);
    print_region("CODE     ", CODE_START, CODE_END);
    print_region("RO DATA  ", RO_DATA_START, RO_DATA_END);
    print_region("RESERVED ", RO_DATA_END, RW_DATA_START);
    print_region("RW DATA  ", RW_DATA_START, RW_DATA_END);
    print_region("RESERVED ", RW_DATA_END, STACK_END);
    print_region("STACK    ", STACK_END, STACK_START);
    print_region(".data LMA/VMA ",
                 (uintptr_t)__data_load_start, (uintptr_t)__data_start);

    assert_msg((boot_data_cookie == BOOT_DATA_COOKIE_INITIAL),
               "initialized data cookie is corrupted");
    assert((boot_bss_cookie == BOOT_BSS_INITIAL));
    assert_msg((stack_pointer >= STACK_END),
               "stack pointer is below the allocated stack");
    assert_msg((stack_pointer < STACK_START),
               "stack pointer is above the allocated stack");
    assert_msg(((stack_pointer & (ARCH_STACK_ALIGNMENT - U(1))) == U(0)),
               "stack pointer does not satisfy ABI alignment");

    boot_status = BOOT_STATUS_READY;
    boot_ready();
    console_puts("BOOT OK ");
    console_puts(image_name);
    console_puts(" [");
    console_puts(arch);
    console_puts("]\n");
}
