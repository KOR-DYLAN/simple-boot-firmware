/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/mps2-an385/include/platform_def.h
 * Brief: Define UART address, clock, and baud rate for mps2-an385.
 */

#ifndef QEMU_MPS2_AN385_PLATFORM_DEF_H
#define QEMU_MPS2_AN385_PLATFORM_DEF_H

#include "compiler_rt.h"

/* Platform identity ------------------------------------------------------ */
#define PLATFORM_NAME             "qemu/mps2-an385"

/* UART configuration macros ---------------------------------------------- */
#define PLATFORM_UART_BASE        U(0x40004000)
#define PLATFORM_UART_CLOCK_HZ    U(25000000)
#define PLATFORM_UART_BAUDRATE    U(115200)

/* Interrupt vector configuration ----------------------------------------- */
#define PLATFORM_IRQ_COUNT        U(32)
#define PLATFORM_VECTOR_ALIGNMENT U(256)
#define PLATFORM_IRQ_NUMBERS      \
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, \
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31

#endif /* QEMU_MPS2_AN385_PLATFORM_DEF_H */
