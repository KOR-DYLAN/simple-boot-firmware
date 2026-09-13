/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/cmsdk_apb_uart/cmsdk_apb_uart.c
 * Brief: Initialize and transmit through a CMSDK APB UART.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/cmsdk_apb_uart.h"

/* Register and control macros -------------------------------------------- */
#define CMSDK_DATA    0x00
#define CMSDK_STATE   0x04
#define CMSDK_CTRL    0x08
#define CMSDK_BAUDDIV 0x10

#define CMSDK_TX_FULL        (1 << 0)
#define CMSDK_CTRL_DISABLED  0
#define CMSDK_CTRL_TX_ENABLE (1 << 0)

/* Register access helper ------------------------------------------------- */
static volatile uint32_t *reg(const struct console_device *device,
                             uintptr_t offset)
{
    return (volatile uint32_t *)(device->base + offset);
}

/* Driver implementation -------------------------------------------------- */
static void cmsdk_init(const struct console_device *device)
{
    *reg(device, CMSDK_CTRL) = CMSDK_CTRL_DISABLED;
    *reg(device, CMSDK_BAUDDIV) =
        (device->clock_hz + device->baudrate / CONSOLE_BAUD_ROUNDING_DENOMINATOR) /
        device->baudrate;
    *reg(device, CMSDK_CTRL) = CMSDK_CTRL_TX_ENABLE;
}

static void cmsdk_putc(const struct console_device *device, char character)
{
    while ((*reg(device, CMSDK_STATE) & CMSDK_TX_FULL) != 0) {
        /* Wait for room in the transmit buffer. */
    }
    *reg(device, CMSDK_DATA) = (uint32_t)(unsigned char)character;
}

/* Driver operations ------------------------------------------------------ */
const struct console_ops cmsdk_apb_uart_console_ops = {
    .init = cmsdk_init,
    .putc = cmsdk_putc,
};
