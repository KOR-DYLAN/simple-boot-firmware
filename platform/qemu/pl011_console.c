/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/pl011_console.c
 * Brief: Validate and bind a QEMU PL011 console device.
 */

/* Includes --------------------------------------------------------------- */
#include "compiler_rt.h"
#include "driver/pl011.h"
#include "platform_def.h"

/* Configuration checks --------------------------------------------------- */
COMPILER_STATIC_ASSERT(
    ((PLATFORM_UART_BAUDRATE > 0) && (PLATFORM_UART_CLOCK_HZ > 0)),
    "UART clock and baud rate must be nonzero"
);
COMPILER_STATIC_ASSERT(
    (PLATFORM_UART_CLOCK_HZ <=
     ((UINT32_MAX - (PLATFORM_UART_BAUDRATE /
                     CONSOLE_BAUD_ROUNDING_DENOMINATOR)) /
      PL011_BAUD_CLOCK_SCALE)),
    "PL011 baud divider arithmetic overflow"
);

/* Platform console binding ----------------------------------------------- */
const struct console_device platform_console = {
    .ops = &pl011_console_ops,
    .base = PLATFORM_UART_BASE,
    .clock_hz = PLATFORM_UART_CLOCK_HZ,
    .baudrate = PLATFORM_UART_BAUDRATE,
};
