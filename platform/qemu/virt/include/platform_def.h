/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/virt/include/platform_def.h
 * Brief: Define UART address, clock, and baud rate for virt.
 */

#ifndef QEMU_VIRT_PLATFORM_DEF_H
#define QEMU_VIRT_PLATFORM_DEF_H

#include "compiler_rt.h"

/* Platform identity ------------------------------------------------------ */
#define PLATFORM_NAME             "qemu/virt"

/* UART configuration macros ---------------------------------------------- */
#define PLATFORM_UART_BASE        U(0x09000000)
#define PLATFORM_UART_CLOCK_HZ    U(24000000)
#define PLATFORM_UART_BAUDRATE    U(115200)

#endif /* QEMU_VIRT_PLATFORM_DEF_H */
