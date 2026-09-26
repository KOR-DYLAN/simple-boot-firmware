/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/assert.h
 * Brief: Route assertions with optional messages to the panic layer.
 */

#ifndef SIMPLE_BOOT_ASSERT_H
#define SIMPLE_BOOT_ASSERT_H

/* Includes --------------------------------------------------------------- */
#include "panic.h"

/* Assertion macros ------------------------------------------------------- */
#define assert(condition)                                                    \
    do {                                                                     \
        if (!(condition)) {                                                  \
            panic(#condition, __FILE__, __func__, (uint32_t)__LINE__);        \
        }                                                                    \
    } while (0)

#define assert_msg(condition, message)                                       \
    do {                                                                     \
        if (!(condition)) {                                                  \
            panic_msg(#condition, (message), __FILE__, __func__,             \
                      (uint32_t)__LINE__);                                    \
        }                                                                    \
    } while (0)

#endif /* SIMPLE_BOOT_ASSERT_H */
