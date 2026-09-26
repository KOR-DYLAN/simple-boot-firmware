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
long strtol(const char *COMPILER_RESTRICT text,
            char **COMPILER_RESTRICT end, int base);
unsigned long strtoul(const char *COMPILER_RESTRICT text,
                      char **COMPILER_RESTRICT end, int base);

#endif /* SIMPLE_BOOT_STDLIB_H */
