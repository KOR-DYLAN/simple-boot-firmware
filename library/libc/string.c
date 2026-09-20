/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/libc/string.c
 * Brief: Implement freestanding memory and string operations.
 */

/* Includes --------------------------------------------------------------- */
#include "stdint.h"
#include "string.h"

/* Memory operations ------------------------------------------------------ */
void *memcpy(void *restrict destination, const void *restrict source, size_t count)
{
    unsigned char *dest = destination;
    const unsigned char *src = source;

    for (size_t i = 0; i < count; ++i) {
        dest[i] = src[i];
    }
    return destination;
}

void *memmove(void *destination, const void *source, size_t count)
{
    unsigned char *dest = destination;
    const unsigned char *src = source;

    /* Integer address ordering also covers separate objects on these targets. */
    if ((uintptr_t)dest <= (uintptr_t)src) {
        for (size_t i = 0; i < count; ++i) {
            dest[i] = src[i];
        }
    } else {
        while (count != 0) {
            --count;
            dest[count] = src[count];
        }
    }
    return destination;
}

void *memset(void *destination, int value, size_t count)
{
    unsigned char *dest = destination;

    for (size_t i = 0; i < count; ++i) {
        dest[i] = (unsigned char)value;
    }
    return destination;
}

int memcmp(const void *left, const void *right, size_t count)
{
    const unsigned char *a = left;
    const unsigned char *b = right;

    for (size_t i = 0; i < count; ++i) {
        if (a[i] != b[i]) {
            return (int)a[i] - (int)b[i];
        }
    }
    return 0;
}

/* String operations ------------------------------------------------------ */
size_t strlen(const char *text)
{
    size_t size = 0;

    while (text[size] != '\0') {
        ++size;
    }
    return size;
}
