/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/stdbool.h
 * Brief: Define the C boolean compatibility macros.
 */

#ifndef SIMPLE_BOOT_STDBOOL_H
#define SIMPLE_BOOT_STDBOOL_H

/* Boolean compatibility macros ------------------------------------------ */
/* C99 requires true and false to be int-compatible constant expressions. */
#define bool                          _Bool
#define true                          1
#define false                         0
#define __bool_true_false_are_defined 1

#endif /* SIMPLE_BOOT_STDBOOL_H */
