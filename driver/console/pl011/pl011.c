/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: driver/console/pl011/pl011.c
 * Brief: Initialize and transmit through a PL011 UART.
 */

/* Includes --------------------------------------------------------------- */
#include "driver/pl011.h"

/* Register and control macros -------------------------------------------- */
#define PL011_DR   0x00
#define PL011_FR   0x18
#define PL011_IBRD 0x24
#define PL011_FBRD 0x28
#define PL011_LCRH 0x2c
#define PL011_CR   0x30
#define PL011_IMSC 0x38
#define PL011_ICR  0x44

#define PL011_TX_FULL              (1 << 5)
#define PL011_CR_DISABLED          0
#define PL011_CR_UART_ENABLE       (1 << 0)
#define PL011_CR_TX_ENABLE         (1 << 8)
#define PL011_INTERRUPTS_DISABLED  0
#define PL011_INTERRUPTS_CLEAR_ALL 0x7ff
#define PL011_LCRH_FIFO_ENABLE     (1 << 4)
#define PL011_LCRH_WORD_8BIT       (3 << 5)

/* Register access helper ------------------------------------------------- */
static volatile uint32_t *reg(const struct console_device *device,
                             uintptr_t offset)
{
    return (volatile uint32_t *)(device->base + offset);
}

/* Driver implementation -------------------------------------------------- */
static void pl011_init(const struct console_device *device)
{
    /* Round clock / (16 * baud) to a divider with six fractional bits. */
    uint32_t divisor =
        (device->clock_hz * PL011_BAUD_CLOCK_SCALE +
         device->baudrate / CONSOLE_BAUD_ROUNDING_DENOMINATOR) /
        device->baudrate;

    *reg(device, PL011_CR) = PL011_CR_DISABLED;
    *reg(device, PL011_IMSC) = PL011_INTERRUPTS_DISABLED;
    *reg(device, PL011_ICR) = PL011_INTERRUPTS_CLEAR_ALL;
    *reg(device, PL011_IBRD) = divisor >> PL011_BAUD_FRACTION_BITS;
    *reg(device, PL011_FBRD) = divisor & PL011_BAUD_FRACTION_MASK;
    *reg(device, PL011_LCRH) = PL011_LCRH_WORD_8BIT | PL011_LCRH_FIFO_ENABLE;
    *reg(device, PL011_CR) = PL011_CR_UART_ENABLE | PL011_CR_TX_ENABLE;
}

static void pl011_putc(const struct console_device *device, char character)
{
    while ((*reg(device, PL011_FR) & PL011_TX_FULL) != 0) {
        /* Wait for room in the transmit buffer. */
    }
    *reg(device, PL011_DR) = (uint32_t)(unsigned char)character;
}

/* Driver operations ------------------------------------------------------ */
const struct console_ops pl011_console_ops = {
    .init = pl011_init,
    .putc = pl011_putc,
};
