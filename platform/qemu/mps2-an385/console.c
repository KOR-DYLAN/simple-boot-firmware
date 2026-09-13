/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/mps2-an385/console.c
 * Brief: Validate and bind the QEMU mps2-an385 console device.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/cmsdk_apb_uart.h"
#include "platform_def.h"

/* Configuration checks --------------------------------------------------- */
_Static_assert(PLATFORM_UART_BAUDRATE > 0 && PLATFORM_UART_CLOCK_HZ > 0,
               "UART clock and baud rate must be nonzero");
_Static_assert(PLATFORM_UART_CLOCK_HZ <=
               UINT32_MAX - PLATFORM_UART_BAUDRATE / CONSOLE_BAUD_ROUNDING_DENOMINATOR,
               "CMSDK baud divider arithmetic overflow");

/* Platform console binding ----------------------------------------------- */
const struct console_device platform_console = {
    .ops = &cmsdk_apb_uart_console_ops,
    .base = PLATFORM_UART_BASE,
    .clock_hz = PLATFORM_UART_CLOCK_HZ,
    .baudrate = PLATFORM_UART_BAUDRATE,
};
