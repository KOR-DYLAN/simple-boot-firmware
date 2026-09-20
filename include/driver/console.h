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
#include "stdint.h"

/* Macro definitions ------------------------------------------------------ */
#define CONSOLE_BAUD_ROUNDING_DENOMINATOR 2

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
extern const struct console_device platform_console;

/* Public API ------------------------------------------------------------- */
void console_init(void);
void console_putc(char character);
void console_puts(const char *text);
void console_hex(uintptr_t value);

#endif /* SIMPLE_BOOT_CONSOLE_H */
