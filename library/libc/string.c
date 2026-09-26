/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/libc/string.c
 * Brief: Implement freestanding memory and string operations.
 */

/* Includes --------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Memory operations ------------------------------------------------------ */
void *memcpy(void *COMPILER_RESTRICT destination,
             const void *COMPILER_RESTRICT source, size_t count)
{
    unsigned char *dest = destination;
    const unsigned char *src = source;
    size_t index;

    for (index = U(0); index < count; ++index) {
        dest[index] = src[index];
    }

    return destination;
}

void *memmove(void *destination, const void *source, size_t count)
{
    unsigned char *dest = destination;
    const unsigned char *src = source;
    size_t index;

    /* Integer address ordering also covers separate objects on these targets. */
    if ((uintptr_t)dest <= (uintptr_t)src) {
        for (index = U(0); index < count; ++index) {
            dest[index] = src[index];
        }
    } else {
        while (count != U(0)) {
            --count;
            dest[count] = src[count];
        }
    }

    return destination;
}

void *memset(void *destination, int value, size_t count)
{
    unsigned char *dest = destination;
    size_t index;

    for (index = U(0); index < count; ++index) {
        dest[index] = (unsigned char)value;
    }

    return destination;
}

int memcmp(const void *left, const void *right, size_t count)
{
    const unsigned char *a = left;
    const unsigned char *b = right;
    int result = 0;
    size_t index = U(0);

    /* Stop at the first mismatch while preserving a single function exit. */
    while ((index < count) && (result == 0)) {
        if (a[index] != b[index]) {
            result = (int)a[index] - (int)b[index];
        }
        ++index;
    }

    return result;
}

/* String operations ------------------------------------------------------ */
size_t strlen(const char *text)
{
    size_t size = U(0);

    while (text[size] != '\0') {
        ++size;
    }

    return size;
}

size_t strnlen(const char *text, size_t maximum)
{
    size_t size = U(0);

    while ((size < maximum) && (text[size] != '\0')) {
        ++size;
    }

    return size;
}

char *strcpy(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source)
{
    char *result = destination;
    bool complete = false;

    while (!complete) {
        *destination = *source;
        complete = (*source == '\0');
        ++destination;
        ++source;
    }

    return result;
}

char *strncpy(char *COMPILER_RESTRICT destination,
              const char *COMPILER_RESTRICT source, size_t count)
{
    char *result = destination;
    size_t index = U(0);

    while ((index < count) && (source[index] != '\0')) {
        destination[index] = source[index];
        ++index;
    }

    while (index < count) {
        destination[index++] = '\0';
    }

    return result;
}

char *strcat(char *COMPILER_RESTRICT destination,
             const char *COMPILER_RESTRICT source)
{
    strcpy(destination + strlen(destination), source);

    return destination;
}

int strcmp(const char *left, const char *right)
{
    while ((*left != '\0') && (*left == *right)) {
        ++left;
        ++right;
    }

    return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

int strncmp(const char *left, const char *right, size_t count)
{
    unsigned char a;
    unsigned char b;
    int result = 0;
    size_t index = U(0);
    bool complete = false;

    while ((index < count) && (!complete)) {
        a = (unsigned char)left[index];
        b = (unsigned char)right[index];

        if ((a != b) || (a == '\0')) {
            result = (int)a - (int)b;
            complete = true;
        }

        ++index;
    }

    return result;
}

char *strchr(const char *text, int character)
{
    char target = (char)character;
    char *result = NULL;
    bool complete = false;

    while (!complete) {
        if (*text == target) {
            result = (char *)text;
            complete = true;
        } else if (*text == '\0') {
            complete = true;
        } else {
            ++text;
        }
    }

    return result;
}

char *strrchr(const char *text, int character)
{
    const char *result = NULL;
    char target = (char)character;
    bool complete = false;

    while (!complete) {
        if (*text == target) {
            result = text;
        }

        /* Include the terminator in the search, as required for '\0'. */
        complete = (*text == '\0');
        ++text;
    }

    return (char *)result;
}
