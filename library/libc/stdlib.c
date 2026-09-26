/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/libc/stdlib.c
 * Brief: Implement freestanding integer conversion operations.
 */

/* Includes --------------------------------------------------------------- */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* Error state ------------------------------------------------------------ */
int errno;

/* Private helpers -------------------------------------------------------- */
static unsigned long divide_ulong(unsigned long dividend, unsigned int divisor)
{
    unsigned long quotient = UL(0);
    unsigned long shifted_divisor = divisor;
    unsigned long bit = UL(1);

    /* Keep conversion independent from compiler-provided divide helpers. */
    while ((shifted_divisor <= dividend) &&
           (shifted_divisor <= (ULONG_MAX >> U(1)))) {
        shifted_divisor <<= U(1);
        bit <<= U(1);
    }

    while (bit != UL(0)) {
        if (dividend >= shifted_divisor) {
            dividend -= shifted_divisor;
            quotient |= bit;
        }
        shifted_divisor >>= U(1);
        bit >>= U(1);
    }

    return quotient;
}

static unsigned int digit_value(int character)
{
    unsigned int result = U(36);

    if ((character >= '0') && (character <= '9')) {
        result = (unsigned int)(character - '0');
    } else if ((character >= 'a') && (character <= 'z')) {
        result = (unsigned int)(character - 'a') + U(10);
    } else if ((character >= 'A') && (character <= 'Z')) {
        result = (unsigned int)(character - 'A') + U(10);
    } else {
        /* Keep the out-of-range sentinel. */
    }

    return result;
}

static unsigned long parse_unsigned(const char *text, char **end, int base,
                                    bool *negative, bool *overflow)
{
    const char *cursor = text;
    const char *digits;
    unsigned long value = UL(0);
    unsigned int digit;

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }

    *negative = (*cursor == '-');
    if ((*cursor == '+') || (*cursor == '-')) {
        ++cursor;
    }

    if (((base == 0) || (base == 16)) && (cursor[U(0)] == '0') &&
        ((cursor[U(1)] == 'x') || (cursor[U(1)] == 'X')) &&
        (digit_value((unsigned char)cursor[U(2)]) < U(16))) {
        cursor += U(2);
        base = 16;
    } else if (base == 0) {
        base = (cursor[U(0)] == '0') ? 8 : 10;
    }

    digits = cursor;
    while (digit_value((unsigned char)*cursor) < (unsigned int)base) {
        digit = digit_value((unsigned char)*cursor);
        ++cursor;
        if (value > divide_ulong(ULONG_MAX - digit, (unsigned int)base)) {
            /* Continue scanning after saturation so end points past all digits. */
            *overflow = true;
            value = ULONG_MAX;
        } else if (!(*overflow)) {
            value = (value * (unsigned int)base) + digit;
        }
    }

    if (end != NULL) {
        *end = (char *)((cursor == digits) ? text : cursor);
    }

    return value;
}

/* Public API ------------------------------------------------------------- */
unsigned long strtoul(const char *COMPILER_RESTRICT text,
                      char **COMPILER_RESTRICT end, int base)
{
    bool negative = false;
    bool overflow = false;
    unsigned long value;

    if ((base < 0) || (base == 1) || (base > 36)) {
        errno = EINVAL;
        if (end != NULL) {
            *end = (char *)text;
        }
        value = UL(0);
    } else {
        value = parse_unsigned(text, end, base, &negative, &overflow);
        if (overflow) {
            errno = ERANGE;
            value = ULONG_MAX;
        } else if (negative) {
            value = UL(0) - value;
        } else {
            /* The parsed value is already the result. */
        }
    }

    return value;
}

long strtol(const char *COMPILER_RESTRICT text,
            char **COMPILER_RESTRICT end, int base)
{
    bool negative = false;
    bool overflow = false;
    unsigned long value;
    unsigned long limit;
    long result;

    if ((base < 0) || (base == 1) || (base > 36)) {
        errno = EINVAL;
        if (end != NULL) {
            *end = (char *)text;
        }
        result = L(0);
    } else {
        value = parse_unsigned(text, end, base, &negative, &overflow);
        limit = negative ? ((unsigned long)LONG_MAX + UL(1))
                         : (unsigned long)LONG_MAX;
        if (overflow || (value > limit)) {
            errno = ERANGE;
            result = negative ? LONG_MIN : LONG_MAX;
        } else if (negative) {
            result = (value == ((unsigned long)LONG_MAX + UL(1)))
                         ? LONG_MIN
                         : -(long)value;
        } else {
            result = (long)value;
        }
    }

    return result;
}
