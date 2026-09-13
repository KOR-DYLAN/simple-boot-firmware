/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/driver/cmsdk_apb_uart.h
 * Brief: Declare the CMSDK APB UART console backend.
 */

#ifndef SIMPLE_BOOT_CMSDK_UART_H
#define SIMPLE_BOOT_CMSDK_UART_H

/* Includes --------------------------------------------------------------- */
#include "driver/console.h"

/* Driver operations ------------------------------------------------------ */
extern const struct console_ops cmsdk_apb_uart_console_ops;

#endif /* SIMPLE_BOOT_CMSDK_UART_H */
