/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/virt/console.c
 * Brief: Validate and bind the QEMU virt console device.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/pl011.h"
#include "platform_def.h"

/* Configuration checks --------------------------------------------------- */
_Static_assert(PLATFORM_UART_BAUDRATE > 0 && PLATFORM_UART_CLOCK_HZ > 0,
               "UART clock and baud rate must be nonzero");
_Static_assert(PLATFORM_UART_CLOCK_HZ <=
               (UINT32_MAX - PLATFORM_UART_BAUDRATE /
                CONSOLE_BAUD_ROUNDING_DENOMINATOR) / PL011_BAUD_CLOCK_SCALE,
               "PL011 baud divider arithmetic overflow");

/* Platform console binding ----------------------------------------------- */
const struct console_device platform_console = {
    .ops = &pl011_console_ops,
    .base = PLATFORM_UART_BASE,
    .clock_hz = PLATFORM_UART_CLOCK_HZ,
    .baudrate = PLATFORM_UART_BAUDRATE,
};
