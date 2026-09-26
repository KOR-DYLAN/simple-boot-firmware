/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/limits.h
 * Brief: Define implementation integer limits without toolchain libc headers.
 */

#ifndef SIMPLE_BOOT_LIMITS_H
#define SIMPLE_BOOT_LIMITS_H

#include "compiler_rt.h"

#define CHAR_BIT   __CHAR_BIT__
#define SCHAR_MIN  ((-__SCHAR_MAX__) - 1)
#define SCHAR_MAX  __SCHAR_MAX__
#define UCHAR_MAX  ((SCHAR_MAX * U(2)) + U(1))
#define CHAR_MIN   SCHAR_MIN
#define CHAR_MAX   SCHAR_MAX
#define SHRT_MIN   ((-__SHRT_MAX__) - 1)
#define SHRT_MAX   __SHRT_MAX__
#define USHRT_MAX  ((SHRT_MAX * U(2)) + U(1))
#define INT_MIN    ((-__INT_MAX__) - 1)
#define INT_MAX    __INT_MAX__
#define UINT_MAX   ((INT_MAX * U(2)) + U(1))
#define LONG_MIN   ((-__LONG_MAX__) - L(1))
#define LONG_MAX   __LONG_MAX__
#define ULONG_MAX  ((LONG_MAX * UL(2)) + UL(1))
#define LLONG_MIN  ((-__LONG_LONG_MAX__) - LL(1))
#define LLONG_MAX  __LONG_LONG_MAX__
#define ULLONG_MAX ((LLONG_MAX * ULL(2)) + ULL(1))

#endif /* SIMPLE_BOOT_LIMITS_H */
