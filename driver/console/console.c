/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/console.c
 * Brief: Implement polling console output and hexadecimal formatting.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/console.h"

/* Macro definitions ------------------------------------------------------ */
#define CONSOLE_BITS_PER_BYTE  8
#define CONSOLE_HEX_DIGIT_BITS 4
#define CONSOLE_HEX_DIGIT_MASK ((1 << CONSOLE_HEX_DIGIT_BITS) - 1)

/* Public API ------------------------------------------------------------- */
void console_init(void)
{
    platform_console.ops->init(&platform_console);
}

void console_putc(char character)
{
    platform_console.ops->putc(&platform_console, character);
}

void console_puts(const char *text)
{
    while (*text != '\0') {
        if (*text == '\n') {
            console_putc('\r');
        }
        console_putc(*text++);
    }
}

void console_hex(uintptr_t value)
{
    static const char digits[] = "0123456789abcdef";

    console_puts("0x");
    for (unsigned int shift = sizeof(value) * CONSOLE_BITS_PER_BYTE; shift != 0;) {
        shift -= CONSOLE_HEX_DIGIT_BITS;
        console_putc(digits[(value >> shift) & CONSOLE_HEX_DIGIT_MASK]);
    }
}
