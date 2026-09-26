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
#include <stddef.h>
#include "compiler_rt.h"

/* Public API ------------------------------------------------------------- */
void *memcpy(void *COMPILER_RESTRICT destination,
             const void *COMPILER_RESTRICT source, size_t count);
void *memmove(void *destination, const void *source, size_t count);
void *memset(void *destination, int value, size_t count);
int memcmp(const void *left, const void *right, size_t count);
size_t strlen(const char *text);
size_t strnlen(const char *text, size_t maximum);
char *strcpy(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source);
char *strncpy(char *COMPILER_RESTRICT destination,
              const char *COMPILER_RESTRICT source, size_t count);
char *strcat(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source);
int strcmp(const char *left, const char *right);
int strncmp(const char *left, const char *right, size_t count);
char *strchr(const char *text, int character);
char *strrchr(const char *text, int character);

#endif /* SIMPLE_BOOT_STRING_H */
