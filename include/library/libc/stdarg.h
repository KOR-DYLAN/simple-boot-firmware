/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/stdarg.h
 * Brief: Define compiler-backed variable argument handling.
 */

#ifndef SIMPLE_BOOT_STDARG_H
#define SIMPLE_BOOT_STDARG_H

typedef __builtin_va_list va_list;

/* Variable argument operations ------------------------------------------ */
/**
 * @brief Initialize an argument list.
 *
 * @param arguments Argument list to initialize.
 * @param last Last named parameter before the variable arguments.
 */
#define va_start(arguments, last)    __builtin_va_start(arguments, last)

/**
 * @brief Retrieve the next variable argument.
 *
 * @param arguments Initialized argument list.
 * @param type Expected argument type.
 *
 * @return The next argument converted according to type.
 */
#define va_arg(arguments, type)      __builtin_va_arg(arguments, type)

/**
 * @brief Copy the current traversal state of an argument list.
 *
 * @param destination Argument list to initialize as a copy.
 * @param source Initialized argument list to copy.
 */
#define va_copy(destination, source) __builtin_va_copy(destination, source)

/**
 * @brief Release resources associated with an argument list.
 *
 * @param arguments Initialized argument list to release.
 */
#define va_end(arguments)            __builtin_va_end(arguments)

#endif /* SIMPLE_BOOT_STDARG_H */
