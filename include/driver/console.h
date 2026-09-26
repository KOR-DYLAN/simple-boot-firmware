/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/driver/console.h
 * Brief: Declare the console API, device configuration, and driver operations.
 */

#ifndef SIMPLE_BOOT_CONSOLE_H
#define SIMPLE_BOOT_CONSOLE_H

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "compiler_rt.h"

/* Macro definitions ------------------------------------------------------ */
#define CONSOLE_BAUD_ROUNDING_DENOMINATOR U(2)

/* Type declarations ------------------------------------------------------ */
struct console_device;
struct console_ops {
    void (*init)(const struct console_device *device);
    void (*putc)(const struct console_device *device, char character);
};

/*
 * Immutable platform configuration for polling output after runtime_init. Clock and
 * baud rate must be positive and fit the driver's 32-bit divider arithmetic.
 */
struct console_device {
    const struct console_ops *ops;
    uintptr_t base;
    uint32_t clock_hz;
    uint32_t baudrate;
};

/* Platform device -------------------------------------------------------- */
/**
 * @brief Immutable console device selected by the active platform.
 */
extern const struct console_device platform_console;

/* Public API ------------------------------------------------------------- */
/**
 * @brief Initialize the configured platform console.
 */
void console_init(void);

/**
 * @brief Write one character to the platform console.
 *
 * @param character Character to write.
 */
void console_putc(char character);

/**
 * @brief Write a null-terminated string to the platform console.
 *
 * @param text String to write.
 */
void console_puts(const char *text);

/**
 * @brief Write a pointer-width hexadecimal value to the platform console.
 *
 * @param value Value to write.
 */
void console_hex(uintptr_t value);

#endif /* SIMPLE_BOOT_CONSOLE_H */
