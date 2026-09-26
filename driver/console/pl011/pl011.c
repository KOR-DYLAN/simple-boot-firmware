/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/pl011/pl011.c
 * Brief: Initialize and transmit through a PL011 UART.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/pl011.h"
#include "bit.h"

/* Register and control macros -------------------------------------------- */
#define PL011_DR   U(0x00)
#define PL011_FR   U(0x18)
#define PL011_IBRD U(0x24)
#define PL011_FBRD U(0x28)
#define PL011_LCRH U(0x2c)
#define PL011_CR   U(0x30)
#define PL011_IMSC U(0x38)
#define PL011_ICR  U(0x44)

#define PL011_TX_FULL              BIT32(U(5))
#define PL011_CR_DISABLED          U(0)
#define PL011_CR_UART_ENABLE       BIT32(U(0))
#define PL011_CR_TX_ENABLE         BIT32(U(8))
#define PL011_INTERRUPTS_DISABLED  U(0)
#define PL011_INTERRUPTS_CLEAR_ALL U(0x7ff)
#define PL011_LCRH_FIFO_ENABLE     BIT32(U(4))
#define PL011_LCRH_WORD_8BIT       FIELD_PREP(GENMASK32(U(6), U(5)), U(3))

/* Register access helper ------------------------------------------------- */
static volatile uint32_t *pl011_reg(const struct console_device *device,
                                   uintptr_t offset)
{
    return (volatile uint32_t *)(device->base + offset);
}

/* Driver implementation -------------------------------------------------- */
static void pl011_init(const struct console_device *device)
{
    uint32_t divisor;

    /* Round clock / (16 * baud) to a divider with six fractional bits. */
    divisor =
        (device->clock_hz * PL011_BAUD_CLOCK_SCALE +
         device->baudrate / CONSOLE_BAUD_ROUNDING_DENOMINATOR) /
        device->baudrate;

    *pl011_reg(device, PL011_CR) = PL011_CR_DISABLED;
    *pl011_reg(device, PL011_IMSC) = PL011_INTERRUPTS_DISABLED;
    *pl011_reg(device, PL011_ICR) = PL011_INTERRUPTS_CLEAR_ALL;
    *pl011_reg(device, PL011_IBRD) = divisor >> PL011_BAUD_FRACTION_BITS;
    *pl011_reg(device, PL011_FBRD) = divisor & PL011_BAUD_FRACTION_MASK;
    *pl011_reg(device, PL011_LCRH) =
        PL011_LCRH_WORD_8BIT | PL011_LCRH_FIFO_ENABLE;
    *pl011_reg(device, PL011_CR) =
        PL011_CR_UART_ENABLE | PL011_CR_TX_ENABLE;
}

static void pl011_putc(const struct console_device *device, char character)
{
    while ((*pl011_reg(device, PL011_FR) & PL011_TX_FULL) != U(0)) {
        /* Wait for room in the transmit buffer. */
    }

    *pl011_reg(device, PL011_DR) = (uint32_t)(unsigned char)character;
}

/* Driver operations ------------------------------------------------------ */
const struct console_ops pl011_console_ops = {
    .init = pl011_init,
    .putc = pl011_putc,
};
