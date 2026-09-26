/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/console.c
 * Brief: Implement polling console output and hexadecimal formatting.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/console.h"
#include "bit.h"
#include <stdio.h>

/* Macro definitions ------------------------------------------------------ */
#define CONSOLE_BITS_PER_BYTE  U(8)
#define CONSOLE_HEX_DIGIT_BITS U(4)
#define CONSOLE_HEX_DIGIT_MASK GENMASK32(U(3), U(0))

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
    uint32_t shift;

    console_puts("0x");
    for (shift = sizeof(value) * CONSOLE_BITS_PER_BYTE; shift != U(0);) {
        shift -= CONSOLE_HEX_DIGIT_BITS;
        console_putc(digits[(value >> shift) & CONSOLE_HEX_DIGIT_MASK]);
    }
}

int putchar(int character)
{
    if (character == '\n') {
        console_putc('\r');
    }
    console_putc((char)character);
    return (unsigned char)character;
}
