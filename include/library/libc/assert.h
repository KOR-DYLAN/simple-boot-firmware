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
/**
 * @brief Panic when a required runtime condition is false.
 *
 * @param condition Runtime condition that must evaluate as true.
 */
#define assert(condition)                                                    \
    do {                                                                     \
        if (!(condition)) {                                                  \
            panic(#condition, __FILE__, __func__, (uint32_t)__LINE__);        \
        }                                                                    \
    } while (0)

/**
 * @brief Panic with a descriptive message when a condition is false.
 *
 * @param condition Runtime condition that must evaluate as true.
 * @param message Null-terminated diagnostic message.
 */
#define assert_msg(condition, message)                                       \
    do {                                                                     \
        if (!(condition)) {                                                  \
            panic_msg(#condition, (message), __FILE__, __func__,             \
                      (uint32_t)__LINE__);                                    \
        }                                                                    \
    } while (0)

#endif /* SIMPLE_BOOT_ASSERT_H */
