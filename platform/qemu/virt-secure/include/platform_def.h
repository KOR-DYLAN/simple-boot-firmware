/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/virt-secure/include/platform_def.h
 * Brief: Define UART address, clock, and baud rate for virt-secure.
 */

#ifndef QEMU_VIRT_PLATFORM_DEF_H
#define QEMU_VIRT_PLATFORM_DEF_H

/* Platform identity ------------------------------------------------------ */
#define PLATFORM_NAME "qemu/virt-secure"

/* UART configuration macros ---------------------------------------------- */
#define PLATFORM_UART_BASE     0x09000000
#define PLATFORM_UART_CLOCK_HZ 24000000
#define PLATFORM_UART_BAUDRATE 115200
#endif /* QEMU_VIRT_PLATFORM_DEF_H */
