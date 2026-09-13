/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/driver/pl011.h
 * Brief: Declare the PL011 backend and baud-divider constants.
 */

#ifndef SIMPLE_BOOT_PL011_H
#define SIMPLE_BOOT_PL011_H

/* Includes --------------------------------------------------------------- */
#include "driver/console.h"

/* Macro definitions ------------------------------------------------------ */
#define PL011_BAUD_OVERSAMPLE     16
#define PL011_BAUD_FRACTION_BITS  6
#define PL011_BAUD_FRACTION_SCALE (1 << PL011_BAUD_FRACTION_BITS)
#define PL011_BAUD_FRACTION_MASK  (PL011_BAUD_FRACTION_SCALE - 1)
#define PL011_BAUD_CLOCK_SCALE    (PL011_BAUD_FRACTION_SCALE / PL011_BAUD_OVERSAMPLE)

/* Driver operations ------------------------------------------------------ */
extern const struct console_ops pl011_console_ops;

#endif /* SIMPLE_BOOT_PL011_H */
