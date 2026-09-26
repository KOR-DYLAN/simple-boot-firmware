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
/**
 * @brief Write one character to the active console sink.
 *
 * @param character Character value to write.
 *
 * @return The written character converted to unsigned char, or an error value.
 */
int putchar(int character);

/**
 * @brief Write a null-terminated string followed by a newline.
 *
 * @param text Null-terminated string to write.
 *
 * @return Zero on success, or an implementation-defined error value.
 */
int puts(const char *text);

/**
 * @brief Write formatted output to the active console.
 *
 * @param format Null-terminated printf-style format string.
 * @param ... Values consumed according to format.
 *
 * @return Number of characters written, or a negative value on error.
 */
int printf(const char *COMPILER_RESTRICT format, ...) COMPILER_FORMAT_PRINTF(1, 2);

/**
 * @brief Write formatted output to an unbounded character buffer.
 *
 * @param buffer Destination buffer with sufficient capacity.
 * @param format Null-terminated printf-style format string.
 * @param ... Values consumed according to format.
 *
 * @return Number of characters written, excluding the terminator, or negative on error.
 */
int sprintf(char *COMPILER_RESTRICT buffer,
            const char *COMPILER_RESTRICT format, ...)
    COMPILER_FORMAT_PRINTF(2, 3);

/**
 * @brief Write formatted output to a size-limited character buffer.
 *
 * @param buffer Destination buffer, or NULL when size is zero.
 * @param size Total destination capacity including the terminator.
 * @param format Null-terminated printf-style format string.
 * @param ... Values consumed according to format.
 *
 * @return Required character count excluding the terminator, or negative on error.
 */
int snprintf(char *COMPILER_RESTRICT buffer, size_t size,
             const char *COMPILER_RESTRICT format, ...)
    COMPILER_FORMAT_PRINTF(3, 4);

/**
 * @brief Write variadic formatted output to the active console.
 *
 * @param format Null-terminated printf-style format string.
 * @param arguments Variable argument list consumed according to format.
 *
 * @return Number of characters written, or a negative value on error.
 */
int vprintf(const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(1, 0);

/**
 * @brief Write variadic formatted output to an unbounded buffer.
 *
 * @param buffer Destination buffer with sufficient capacity.
 * @param format Null-terminated printf-style format string.
 * @param arguments Variable argument list consumed according to format.
 *
 * @return Number of characters written, excluding the terminator, or negative on error.
 */
int vsprintf(char *COMPILER_RESTRICT buffer,
             const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(2, 0);

/**
 * @brief Write variadic formatted output to a size-limited buffer.
 *
 * @param buffer Destination buffer, or NULL when size is zero.
 * @param size Total destination capacity including the terminator.
 * @param format Null-terminated printf-style format string.
 * @param arguments Variable argument list consumed according to format.
 *
 * @return Required character count excluding the terminator, or negative on error.
 */
int vsnprintf(char *COMPILER_RESTRICT buffer, size_t size,
              const char *COMPILER_RESTRICT format, va_list arguments)
    COMPILER_FORMAT_PRINTF(3, 0);

#endif /* SIMPLE_BOOT_STDIO_H */
