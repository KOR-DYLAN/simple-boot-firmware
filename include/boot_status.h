/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/boot_status.h
 * Brief: Define runtime sentinels and debugger-visible boot results.
 */

#ifndef SIMPLE_BOOT_STATUS_H
#define SIMPLE_BOOT_STATUS_H

/* Macro definitions ------------------------------------------------------ */
/* Runtime initialization sentinels and boot results visible in GDB. */
#define BOOT_DATA_COOKIE_INITIAL 0x12345678
#define BOOT_BSS_INITIAL         0
#define BOOT_STATUS_READY        1
#define BOOT_STATUS_FAILED       0xbad

#endif /* SIMPLE_BOOT_STATUS_H */
