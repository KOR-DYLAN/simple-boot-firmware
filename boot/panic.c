/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/panic.c
 * Brief: Report fatal conditions and halt the current boot image.
 */

/* Includes --------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "arch/panic.h"
#include "boot.h"
#include "boot_status.h"
#include "driver/console.h"
#include "panic.h"

/* Private helpers -------------------------------------------------------- */
static COMPILER_NORETURN void
panic_report(const char *expression, const char *message, bool has_message,
             const char *file, const char *function, uint32_t line,
             uintptr_t program_counter)
{
    boot_status = BOOT_STATUS_FAILED;
    console_init();
    printf("\nPANIC: assertion failed\n");
    if (has_message) {
        printf("message: %s\n", message);
    }
    printf("condition: %s\n", expression);
    printf("location: %s:%u (%s)\n", file, (unsigned int)line, function);
    console_puts("registers:\n");
    arch_panic_dump(program_counter);
    boot_halt();
}

/* Panic entry points ----------------------------------------------------- */
COMPILER_NOINLINE COMPILER_NORETURN void
panic(const char *expression, const char *file, const char *function,
      uint32_t line)
{
    uintptr_t program_counter;

    program_counter = (uintptr_t)COMPILER_RETURN_ADDRESS();
    panic_report(expression, "", false, file, function, line,
                 program_counter);
}

COMPILER_NOINLINE COMPILER_NORETURN void
panic_msg(const char *expression, const char *message, const char *file,
          const char *function, uint32_t line)
{
    uintptr_t program_counter;

    program_counter = (uintptr_t)COMPILER_RETURN_ADDRESS();
    panic_report(expression, message, true, file, function, line,
                 program_counter);
}
