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
/**
 * @brief Copy bytes between non-overlapping objects.
 *
 * @param destination Destination object.
 * @param source Source object.
 * @param count Number of bytes to copy.
 *
 * @return The destination pointer.
 */
void *memcpy(void *COMPILER_RESTRICT destination,
             const void *COMPILER_RESTRICT source, size_t count);

/**
 * @brief Copy bytes between potentially overlapping objects.
 *
 * @param destination Destination object.
 * @param source Source object.
 * @param count Number of bytes to copy.
 *
 * @return The destination pointer.
 */
void *memmove(void *destination, const void *source, size_t count);

/**
 * @brief Fill an object with a byte value.
 *
 * @param destination Destination object.
 * @param value Byte value to store.
 * @param count Number of bytes to fill.
 *
 * @return The destination pointer.
 */
void *memset(void *destination, int value, size_t count);

/**
 * @brief Compare two byte sequences.
 *
 * @param left First object.
 * @param right Second object.
 * @param count Number of bytes to compare.
 *
 * @return A value less than, equal to, or greater than zero by lexical order.
 */
int memcmp(const void *left, const void *right, size_t count);

/**
 * @brief Determine the length of a null-terminated string.
 *
 * @param text String to measure.
 *
 * @return Character count excluding the terminator.
 */
size_t strlen(const char *text);

/**
 * @brief Determine a string length up to a maximum count.
 *
 * @param text String to measure.
 * @param maximum Maximum characters to inspect.
 *
 * @return Character count before the terminator or maximum, whichever comes first.
 */
size_t strnlen(const char *text, size_t maximum);

/**
 * @brief Copy a null-terminated string.
 *
 * @param destination Destination buffer.
 * @param source Source string.
 *
 * @return The destination pointer.
 */
char *strcpy(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source);

/**
 * @brief Copy and pad a string to a fixed character count.
 *
 * @param destination Destination buffer.
 * @param source Source string.
 * @param count Number of characters to write.
 *
 * @return The destination pointer.
 */
char *strncpy(char *COMPILER_RESTRICT destination,
              const char *COMPILER_RESTRICT source, size_t count);

/**
 * @brief Append a string to a null-terminated destination string.
 *
 * @param destination Destination string with sufficient capacity.
 * @param source Source string.
 *
 * @return The destination pointer.
 */
char *strcat(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source);

/**
 * @brief Compare two null-terminated strings.
 *
 * @param left First string.
 * @param right Second string.
 *
 * @return A value less than, equal to, or greater than zero by lexical order.
 */
int strcmp(const char *left, const char *right);

/**
 * @brief Compare two strings up to a maximum character count.
 *
 * @param left First string.
 * @param right Second string.
 * @param count Maximum characters to compare.
 *
 * @return A value less than, equal to, or greater than zero by lexical order.
 */
int strncmp(const char *left, const char *right, size_t count);

/**
 * @brief Find the first occurrence of a character in a string.
 *
 * @param text String to search.
 * @param character Character to find.
 *
 * @return Pointer to the matching character, or NULL when absent.
 */
char *strchr(const char *text, int character);

/**
 * @brief Find the last occurrence of a character in a string.
 *
 * @param text String to search.
 * @param character Character to find.
 *
 * @return Pointer to the matching character, or NULL when absent.
 */
char *strrchr(const char *text, int character);

#endif /* SIMPLE_BOOT_STRING_H */
