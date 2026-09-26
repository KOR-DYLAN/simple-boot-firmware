/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/stdio.h
 * Brief: Declare freestanding formatted output operations.
 */

#ifndef SIMPLE_BOOT_STDIO_H
#define SIMPLE_BOOT_STDIO_H

/* Includes --------------------------------------------------------------- */
#include "compiler_rt.h"
#include <stdarg.h>
#include <stddef.h>

/* Public API ------------------------------------------------------------- */
int putchar(int character);
int puts(const char *text);
int printf(const char *COMPILER_RESTRICT format, ...) COMPILER_FORMAT_PRINTF(1, 2);
int sprintf(char *COMPILER_RESTRICT buffer,
            const char *COMPILER_RESTRICT format, ...)
    COMPILER_FORMAT_PRINTF(2, 3);
int snprintf(char *COMPILER_RESTRICT buffer, size_t size,
             const char *COMPILER_RESTRICT format, ...)
    COMPILER_FORMAT_PRINTF(3, 4);
int vprintf(const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(1, 0);
int vsprintf(char *COMPILER_RESTRICT buffer,
             const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(2, 0);
int vsnprintf(char *COMPILER_RESTRICT buffer, size_t size,
              const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(3, 0);

#endif /* SIMPLE_BOOT_STDIO_H */
