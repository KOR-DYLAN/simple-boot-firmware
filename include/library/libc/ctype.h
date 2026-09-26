/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/ctype.h
 * Brief: Declare locale-independent ASCII character classification.
 */

#ifndef SIMPLE_BOOT_CTYPE_H
#define SIMPLE_BOOT_CTYPE_H

int isalnum(int character);
int isalpha(int character);
int isdigit(int character);
int isspace(int character);
int isxdigit(int character);
int tolower(int character);
int toupper(int character);

#endif /* SIMPLE_BOOT_CTYPE_H */
