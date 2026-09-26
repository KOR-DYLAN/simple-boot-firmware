/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/ctype.h
 * Brief: Declare locale-independent ASCII character classification.
 */

#ifndef SIMPLE_BOOT_CTYPE_H
#define SIMPLE_BOOT_CTYPE_H

/* Public API ------------------------------------------------------------- */
/**
 * @brief Test whether a character is an ASCII letter or decimal digit.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return Non-zero when character is alphanumeric; otherwise zero.
 */
int isalnum(int character);

/**
 * @brief Test whether a character is an ASCII letter.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return Non-zero when character is alphabetic; otherwise zero.
 */
int isalpha(int character);

/**
 * @brief Test whether a character is an ASCII decimal digit.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return Non-zero when character is a decimal digit; otherwise zero.
 */
int isdigit(int character);

/**
 * @brief Test whether a character is ASCII white space.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return Non-zero when character is white space; otherwise zero.
 */
int isspace(int character);

/**
 * @brief Test whether a character is an ASCII hexadecimal digit.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return Non-zero when character is a hexadecimal digit; otherwise zero.
 */
int isxdigit(int character);

/**
 * @brief Convert an ASCII uppercase letter to lowercase.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return The lowercase equivalent, or character when no conversion applies.
 */
int tolower(int character);

/**
 * @brief Convert an ASCII lowercase letter to uppercase.
 *
 * @param character Character value representable as unsigned char, or EOF.
 *
 * @return The uppercase equivalent, or character when no conversion applies.
 */
int toupper(int character);

#endif /* SIMPLE_BOOT_CTYPE_H */
