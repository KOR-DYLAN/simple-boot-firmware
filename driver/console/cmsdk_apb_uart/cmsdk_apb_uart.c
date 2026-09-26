/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/cmsdk_apb_uart/cmsdk_apb_uart.c
 * Brief: Initialize and transmit through a CMSDK APB UART.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/cmsdk_apb_uart.h"
#include "bit.h"

/* Register and control macros -------------------------------------------- */
#define CMSDK_DATA    U(0x00)
#define CMSDK_STATE   U(0x04)
#define CMSDK_CTRL    U(0x08)
#define CMSDK_BAUDDIV U(0x10)

#define CMSDK_TX_FULL        BIT32(U(0))
#define CMSDK_CTRL_DISABLED  U(0)
#define CMSDK_CTRL_TX_ENABLE BIT32(U(0))

/* Register access helper ------------------------------------------------- */
static volatile uint32_t *cmsdk_reg(const struct console_device *device,
                                   uintptr_t offset)
{
    return (volatile uint32_t *)(device->base + offset);
}

/* Driver implementation -------------------------------------------------- */
static void cmsdk_init(const struct console_device *device)
{
    uint32_t baud_divisor;

    baud_divisor =
        (device->clock_hz + device->baudrate / CONSOLE_BAUD_ROUNDING_DENOMINATOR) /
        device->baudrate;

    *cmsdk_reg(device, CMSDK_CTRL) = CMSDK_CTRL_DISABLED;
    *cmsdk_reg(device, CMSDK_BAUDDIV) = baud_divisor;
    *cmsdk_reg(device, CMSDK_CTRL) = CMSDK_CTRL_TX_ENABLE;
}

static void cmsdk_putc(const struct console_device *device, char character)
{
    while ((*cmsdk_reg(device, CMSDK_STATE) & CMSDK_TX_FULL) != U(0)) {
        /* Wait for room in the transmit buffer. */
    }
    *cmsdk_reg(device, CMSDK_DATA) = (uint32_t)(unsigned char)character;
}

/* Driver operations ------------------------------------------------------ */
const struct console_ops cmsdk_apb_uart_console_ops = {
    .init = cmsdk_init,
    .putc = cmsdk_putc,
};
