/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/stdlib.h
 * Brief: Declare freestanding integer conversion operations.
 */

#ifndef SIMPLE_BOOT_STDLIB_H
#define SIMPLE_BOOT_STDLIB_H

/* Includes --------------------------------------------------------------- */
#include "compiler_rt.h"

/* Public API ------------------------------------------------------------- */
/**
 * @brief Convert the initial portion of a string to a signed integer.
 *
 * @param text Null-terminated input string.
 * @param end Receives the first unconverted character; may be NULL.
 * @param base Numeric base from 2 through 36, or zero for automatic detection.
 *
 * @return Converted value, saturated on range error, or zero on invalid input.
 */
long strtol(const char *COMPILER_RESTRICT text,
            char **COMPILER_RESTRICT end, int base);

/**
 * @brief Convert the initial portion of a string to an unsigned integer.
 *
 * @param text Null-terminated input string.
 * @param end Receives the first unconverted character; may be NULL.
 * @param base Numeric base from 2 through 36, or zero for automatic detection.
 *
 * @return Converted value, saturated on range error, or zero on invalid input.
 */
unsigned long strtoul(const char *COMPILER_RESTRICT text,
                      char **COMPILER_RESTRICT end, int base);

#endif /* SIMPLE_BOOT_STDLIB_H */
