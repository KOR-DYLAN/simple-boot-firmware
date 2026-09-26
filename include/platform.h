/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/platform.h
 * Brief: Declare optional platform initialization hooks.
 */

#ifndef SIMPLE_BOOT_PLATFORM_H
#define SIMPLE_BOOT_PLATFORM_H

/* Platform initialization hooks ----------------------------------------- */
void platform_early_init(void);
void platform_arch_init(void);
void platform_init(void);

#endif /* SIMPLE_BOOT_PLATFORM_H */
