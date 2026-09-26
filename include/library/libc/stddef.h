/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/stddef.h
 * Brief: Define project-owned size, pointer-difference, and null types.
 */

#ifndef SIMPLE_BOOT_STDDEF_H
#define SIMPLE_BOOT_STDDEF_H

/* Includes --------------------------------------------------------------- */
#include "compiler_rt.h"

/* Type definitions ------------------------------------------------------- */
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;

/* Macro definitions ------------------------------------------------------ */
#define SIZE_MAX               __SIZE_MAX__
#define NULL                   ((void *)0)
#define offsetof(type, member) COMPILER_OFFSETOF(type, member)

#endif /* SIMPLE_BOOT_STDDEF_H */
