/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/string.h
 * Brief: Declare the freestanding memory and string API.
 */

#ifndef SIMPLE_BOOT_STRING_H
#define SIMPLE_BOOT_STRING_H

/* Includes --------------------------------------------------------------- */
#include "stddef.h"

/* Public API ------------------------------------------------------------- */
void *memcpy(void *restrict destination, const void *restrict source, size_t count);
void *memmove(void *destination, const void *source, size_t count);
void *memset(void *destination, int value, size_t count);
int memcmp(const void *left, const void *right, size_t count);
size_t strlen(const char *text);

#endif /* SIMPLE_BOOT_STRING_H */
