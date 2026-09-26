/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/boot_status.h
 * Brief: Define runtime sentinels and debugger-visible boot results.
 */

#ifndef SIMPLE_BOOT_STATUS_H
#define SIMPLE_BOOT_STATUS_H

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "compiler_rt.h"

/* Macro definitions ------------------------------------------------------ */
/* Runtime initialization sentinels and boot results visible in GDB. */
#define BOOT_DATA_COOKIE_INITIAL U(0x12345678)
#define BOOT_BSS_INITIAL         U(0)
#define BOOT_STATUS_READY        U(1)
#define BOOT_STATUS_FAILED       U(0xbad)

/* Debugger-visible runtime state ----------------------------------------- */
extern volatile uint32_t boot_status;

#endif /* SIMPLE_BOOT_STATUS_H */
