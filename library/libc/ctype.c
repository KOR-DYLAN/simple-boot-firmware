/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: library/libc/ctype.c
 * Brief: Implement locale-independent ASCII character classification.
 */

/* Includes --------------------------------------------------------------- */
#include <ctype.h>

/* Character operations --------------------------------------------------- */
int isdigit(int character)
{
    return (character >= '0') && (character <= '9');
}

int isxdigit(int character)
{
    return (isdigit(character) != 0) ||
           ((character >= 'a') && (character <= 'f')) ||
           ((character >= 'A') && (character <= 'F'));
}

int isalpha(int character)
{
    return ((character >= 'a') && (character <= 'z')) ||
           ((character >= 'A') && (character <= 'Z'));
}

int isalnum(int character)
{
    return (isalpha(character) != 0) || (isdigit(character) != 0);
}

int isspace(int character)
{
    return (character == ' ') || ((character >= '\t') && (character <= '\r'));
}

int tolower(int character)
{
    int result = character;

    if ((character >= 'A') && (character <= 'Z')) {
        result = character + ('a' - 'A');
    }
    return result;
}

int toupper(int character)
{
    int result = character;

    if ((character >= 'a') && (character <= 'z')) {
        result = character - ('a' - 'A');
    }
    return result;
}
