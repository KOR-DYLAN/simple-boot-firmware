/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/panic.h
 * Brief: Declare fatal assertion reporting with optional messages.
 */

#ifndef SIMPLE_BOOT_PANIC_H
#define SIMPLE_BOOT_PANIC_H

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "compiler_rt.h"

/* Panic entry points ----------------------------------------------------- */
/**
 * @brief Report a failed condition and stop execution.
 *
 * @param expression Text of the failed condition.
 * @param file Source file containing the failure.
 * @param function Function containing the failure.
 * @param line Source line containing the failure.
 */
void panic(const char *expression, const char *file, const char *function,
           uint32_t line) COMPILER_NOINLINE COMPILER_NORETURN;
/**
 * @brief Report a failed condition with a message and stop execution.
 *
 * @param expression Text of the failed condition.
 * @param message Null-terminated diagnostic message.
 * @param file Source file containing the failure.
 * @param function Function containing the failure.
 * @param line Source line containing the failure.
 */
void panic_msg(const char *expression, const char *message, const char *file,
               const char *function, uint32_t line)
    COMPILER_NOINLINE COMPILER_NORETURN;

#endif /* SIMPLE_BOOT_PANIC_H */
