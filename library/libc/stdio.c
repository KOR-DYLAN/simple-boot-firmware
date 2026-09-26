/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/libc/stdio.c
 * Brief: Implement formatted output for buffers and the polling console.
 */

/* Includes --------------------------------------------------------------- */
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "compiler_rt.h"

/* Formatting constants --------------------------------------------------- */
#define FORMAT_BASE_OCTAL            U(8)
#define FORMAT_BASE_DECIMAL          U(10)
#define FORMAT_BASE_HEXADECIMAL      U(16)
#define FORMAT_PRECISION_UNSPECIFIED (-1)

/* Formatting state ------------------------------------------------------- */
enum format_length {
    LENGTH_DEFAULT,
    LENGTH_CHAR,
    LENGTH_SHORT,
    LENGTH_LONG,
    LENGTH_LONG_LONG,
    LENGTH_SIZE,
    LENGTH_PTRDIFF,
    LENGTH_MAXIMUM,
};

struct output {
    char *buffer;
    size_t capacity;
    /* Number of characters that would be written without truncation. */
    size_t count;
    bool console;
};

struct format_spec {
    bool left;
    bool plus;
    bool space;
    bool alternate;
    bool zero;
    int width;
    int precision;
    enum format_length length;
};

/* Private helpers -------------------------------------------------------- */
static uintmax_t divide_uintmax(uintmax_t dividend, unsigned int divisor,
                                unsigned int *remainder)
{
    uintmax_t quotient = ULL(0);
    uintmax_t shifted_divisor = divisor;
    uintmax_t bit = ULL(1);

    /* Long division avoids compiler runtime helpers on 32-bit targets. */
    while ((shifted_divisor <= dividend) &&
           (shifted_divisor <= (UINTMAX_MAX >> U(1)))) {
        shifted_divisor <<= U(1);
        bit <<= U(1);
    }

    while (bit != ULL(0)) {
        if (dividend >= shifted_divisor) {
            dividend -= shifted_divisor;
            quotient |= bit;
        }
        shifted_divisor >>= U(1);
        bit >>= U(1);
    }

    *remainder = (unsigned int)dividend;

    return quotient;
}

static void output_character(struct output *output, char character)
{
    if (output->console) {
        putchar((unsigned char)character);
    } else if ((output->capacity != U(0)) &&
               (output->count < (output->capacity - U(1)))) {
        output->buffer[output->count] = character;
    }
    /* snprintf() returns the untruncated length, so always advance count. */
    ++output->count;
}

/* Platforms override this weak sink when console output is available. */
COMPILER_WEAK int putchar(int character)
{

    return (unsigned char)character;
}

int puts(const char *text)
{
    while (*text != '\0') {
        putchar((unsigned char)*text++);
    }

    putchar('\n');

    return 0;
}

static void output_repeat(struct output *output, char character, size_t count)
{
    while (count-- != U(0)) {
        output_character(output, character);
    }
}

static uintmax_t read_unsigned(va_list *arguments, enum format_length length)
{
    uintmax_t result;

    switch (length) {
    case LENGTH_CHAR:
        result = (unsigned char)va_arg(*arguments, unsigned int);
        break;
    case LENGTH_SHORT:
        result = (unsigned short)va_arg(*arguments, unsigned int);
        break;
    case LENGTH_LONG:
        result = va_arg(*arguments, unsigned long);
        break;
    case LENGTH_LONG_LONG:
        result = va_arg(*arguments, unsigned long long);
        break;
    case LENGTH_SIZE:
        result = va_arg(*arguments, size_t);
        break;
    case LENGTH_PTRDIFF:
        result = (uintmax_t)va_arg(*arguments, ptrdiff_t);
        break;
    case LENGTH_MAXIMUM:
        result = va_arg(*arguments, uintmax_t);
        break;
    default:
        result = va_arg(*arguments, unsigned int);
        break;
    }

    return result;
}

static intmax_t read_signed(va_list *arguments, enum format_length length)
{
    intmax_t result;

    switch (length) {
    case LENGTH_CHAR:
        result = (signed char)va_arg(*arguments, int);
        break;
    case LENGTH_SHORT:
        result = (short)va_arg(*arguments, int);
        break;
    case LENGTH_LONG:
        result = va_arg(*arguments, long);
        break;
    case LENGTH_LONG_LONG:
        result = va_arg(*arguments, long long);
        break;
    case LENGTH_SIZE:
    case LENGTH_PTRDIFF:
        result = va_arg(*arguments, ptrdiff_t);
        break;
    case LENGTH_MAXIMUM:
        result = va_arg(*arguments, intmax_t);
        break;
    default:
        result = va_arg(*arguments, int);
        break;
    }

    return result;
}

static void format_integer(struct output *output, uintmax_t value, unsigned int base,
                           bool uppercase, bool left, bool zero, bool alternate,
                           char sign, int width, int precision)
{
    char reversed[sizeof(value) * CHAR_BIT];
    const char *digits;
    const char *prefix = "";
    size_t prefix_size = U(0);
    size_t digit_count = U(0);
    size_t zero_count;
    size_t total;
    unsigned int remainder;

    digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (value == ULL(0)) {
        if (precision != 0) {
            reversed[digit_count++] = '0';
        }
    } else {
        while (value != ULL(0)) {
            value = divide_uintmax(value, base, &remainder);
            reversed[digit_count++] = digits[remainder];
        }
    }

    if ((alternate) && (base == FORMAT_BASE_HEXADECIMAL) &&
        (digit_count != U(0))) {
        prefix = uppercase ? "0X" : "0x";
        prefix_size = U(2);
    } else if ((alternate) && (base == FORMAT_BASE_OCTAL) &&
               ((digit_count == U(0)) ||
                (reversed[digit_count - U(1)] != '0'))) {
        prefix = "0";
        prefix_size = U(1);
    }

    zero_count = (precision > (int)digit_count)
                     ? ((size_t)precision - digit_count)
                     : U(0);
    total = digit_count + zero_count + prefix_size + (size_t)(sign != '\0');
    if ((zero) && (!left) && (precision < 0) && (width > (int)total)) {
        zero_count += (size_t)width - total;
        total = (size_t)width;
    }

    if ((!left) && (width > (int)total)) {
        output_repeat(output, ' ', (size_t)width - total);
    }

    if (sign != '\0') {
        output_character(output, sign);
    }

    while (prefix_size-- != 0) {
        output_character(output, *prefix++);
    }

    output_repeat(output, '0', zero_count);

    while (digit_count-- != 0) {
        output_character(output, reversed[digit_count]);
    }

    if ((left) && (width > (int)total)) {
        output_repeat(output, ' ', (size_t)width - total);
    }
}

static const char *parse_number(const char *format, int *value)
{
    *value = 0;
    while ((*format >= '0') && (*format <= '9')) {
        if (*value <= (INT_MAX - (*format - '0')) / 10) {
            *value = *value * 10 + (*format - '0');
        }

        ++format;
    }

    return format;
}

static const char *parse_flags(const char *format, struct format_spec *spec)
{
    bool parsing = true;

    while (parsing) {
        if (*format == '-') {
            spec->left = true;
        } else if (*format == '+') {
            spec->plus = true;
        } else if (*format == ' ') {
            spec->space = true;
        } else if (*format == '#') {
            spec->alternate = true;
        } else if (*format == '0') {
            spec->zero = true;
        } else {
            parsing = false;
        }
        if (parsing) {
            ++format;
        }
    }

    return format;
}

static const char *parse_width(const char *format, struct format_spec *spec,
                               va_list *arguments)
{
    if (*format == '*') {
        spec->width = va_arg(*arguments, int);
        ++format;
        if (spec->width < 0) {
            spec->left = true;
            spec->width = spec->width == INT_MIN ? INT_MAX : -spec->width;
        }
    } else {
        format = parse_number(format, &spec->width);
    }

    return format;
}

static const char *parse_precision(const char *format, struct format_spec *spec,
                                   va_list *arguments)
{
    if (*format == '.') {
        spec->precision = 0;
        ++format;
        if (*format == '*') {
            spec->precision = va_arg(*arguments, int);
            ++format;
            if (spec->precision < 0) {
                spec->precision = -1;
            }
        } else {
            format = parse_number(format, &spec->precision);
        }
    }

    return format;
}

static const char *parse_length(const char *format, struct format_spec *spec)
{
    if (*format == 'h') {
        spec->length = LENGTH_SHORT;
        ++format;
        if (*format == 'h') {
            spec->length = LENGTH_CHAR;
            ++format;
        }
    } else if (*format == 'l') {
        spec->length = LENGTH_LONG;
        ++format;
        if (*format == 'l') {
            spec->length = LENGTH_LONG_LONG;
            ++format;
        }
    } else if (*format == 'z') {
        spec->length = LENGTH_SIZE;
        ++format;
    } else if (*format == 't') {
        spec->length = LENGTH_PTRDIFF;
        ++format;
    } else if (*format == 'j') {
        spec->length = LENGTH_MAXIMUM;
        ++format;
    } else {
        /* Keep the default length. */
    }

    return format;
}

static void format_signed_value(struct output *output, const struct format_spec *spec,
                                va_list *arguments)
{
    intmax_t signed_value;
    bool negative;
    uintmax_t value;
    char sign = '\0';

    signed_value = read_signed(arguments, spec->length);
    negative = (signed_value < 0);
    value = negative ? (ULL(0) - (uintmax_t)signed_value)
                     : (uintmax_t)signed_value;
    if (negative) {
        sign = '-';
    } else if (spec->plus) {
        sign = '+';
    } else if (spec->space) {
        sign = ' ';
    } else {
        /* No sign prefix is required. */
    }

    format_integer(output, value, FORMAT_BASE_DECIMAL, false, spec->left,
                   spec->zero, false, sign, spec->width, spec->precision);
}

static void format_unsigned_value(struct output *output, const struct format_spec *spec,
                                  char conversion, va_list *arguments)
{
    unsigned int base = FORMAT_BASE_HEXADECIMAL;

    if (conversion == 'o') {
        base = FORMAT_BASE_OCTAL;
    } else if (conversion == 'u') {
        base = FORMAT_BASE_DECIMAL;
    } else {
        /* Hexadecimal is the default for this helper. */
    }

    format_integer(output, read_unsigned(arguments, spec->length), base,
                   (conversion == 'X'), spec->left, spec->zero, spec->alternate,
                   '\0', spec->width, spec->precision);
}

static void format_character(struct output *output, const struct format_spec *spec,
                             va_list *arguments)
{
    if ((!spec->left) && (spec->width > 1)) {
        output_repeat(output, ' ', (size_t)spec->width - U(1));
    }

    output_character(output, (char)va_arg(*arguments, int));

    if ((spec->left) && (spec->width > 1)) {
        output_repeat(output, ' ', (size_t)spec->width - U(1));
    }
}

static void format_string(struct output *output, const struct format_spec *spec,
                          va_list *arguments)
{
    const char *text = va_arg(*arguments, const char *);
    size_t size;
    size_t index;

    if (text == NULL) {
        text = "(null)";
    }

    size = (spec->precision < 0)
               ? strlen(text)
               : strnlen(text, (size_t)spec->precision);
    if ((!spec->left) && (spec->width > (int)size)) {
        output_repeat(output, ' ', (size_t)spec->width - size);
    }

    for (index = U(0); index < size; ++index) {
        output_character(output, text[index]);
    }

    if ((spec->left) && (spec->width > (int)size)) {
        output_repeat(output, ' ', (size_t)spec->width - size);
    }
}

static void format_conversion(struct output *output, const struct format_spec *spec,
                              char conversion, va_list *arguments)
{
    switch (conversion) {
    case 'd':
    case 'i':
        format_signed_value(output, spec, arguments);
        break;
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        format_unsigned_value(output, spec, conversion, arguments);
        break;
    case 'p':
        format_integer(output, (uintptr_t)va_arg(*arguments, void *),
                       FORMAT_BASE_HEXADECIMAL, false, spec->left, spec->zero,
                       true, '\0', spec->width,
                       (spec->precision < 0) ? 1 : spec->precision);
        break;
    case 'c':
        format_character(output, spec, arguments);
        break;
    case 's':
        format_string(output, spec, arguments);
        break;
    case '%':
        output_character(output, '%');
        break;
    default:
        output_character(output, '%');
        if (conversion != '\0') {
            output_character(output, conversion);
        }
        break;
    }
}

static const char *parse_spec(const char *format, struct format_spec *spec,
                              va_list *arguments)
{
    *spec = (struct format_spec){
        .precision = FORMAT_PRECISION_UNSPECIFIED,
    };
    format = parse_flags(format, spec);
    format = parse_width(format, spec, arguments);
    format = parse_precision(format, spec, arguments);
    format = parse_length(format, spec);

    return format;
}

static int format_output(struct output *output, const char *format, va_list arguments)
{
    va_list args;
    struct format_spec spec;
    size_t end;

    va_copy(args, arguments);
    while (*format != '\0') {
        if (*format == '%') {
            format = parse_spec(format + 1, &spec, &args);
            format_conversion(output, &spec, *format, &args);
            if (*format != '\0') {
                ++format;
            }
        } else {
            output_character(output, *format);
            ++format;
        }
    }

    va_end(args);

    if ((!output->console) && (output->capacity != U(0))) {
        end = (output->count < output->capacity)
                  ? output->count
                  : (output->capacity - U(1));
        output->buffer[end] = '\0';
    }

    return (output->count > INT_MAX) ? -1 : (int)output->count;
}

/* Public API ------------------------------------------------------------- */
int vsnprintf(char *COMPILER_RESTRICT buffer, size_t size,
              const char *COMPILER_RESTRICT format, va_list arguments)
{
    struct output output = { .buffer = buffer, .capacity = size };

    return format_output(&output, format, arguments);
}

int snprintf(char *COMPILER_RESTRICT buffer, size_t size,
             const char *COMPILER_RESTRICT format, ...)
{
    va_list arguments;
    int result;

    va_start(arguments, format);
    result = vsnprintf(buffer, size, format, arguments);
    va_end(arguments);

    return result;
}

int vsprintf(char *COMPILER_RESTRICT buffer,
             const char *COMPILER_RESTRICT format, va_list arguments)
{

    return vsnprintf(buffer, SIZE_MAX, format, arguments);
}

int sprintf(char *COMPILER_RESTRICT buffer,
            const char *COMPILER_RESTRICT format, ...)
{
    va_list arguments;
    int result;

    va_start(arguments, format);
    result = vsprintf(buffer, format, arguments);
    va_end(arguments);

    return result;
}

int vprintf(const char *COMPILER_RESTRICT format, va_list arguments)
{
    struct output output = { .console = true };

    return format_output(&output, format, arguments);
}

int printf(const char *COMPILER_RESTRICT format, ...)
{
    va_list arguments;
    int result;

    va_start(arguments, format);
    result = vprintf(format, arguments);
    va_end(arguments);

    return result;
}
