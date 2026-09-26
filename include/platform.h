/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/platform.h
 * Brief: Declare optional platform initialization hooks.
 */

#ifndef SIMPLE_BOOT_PLATFORM_H
#define SIMPLE_BOOT_PLATFORM_H

/* Platform lifecycle hooks ----------------------------------------------- */
/*
 * The boot layer provides one weak no-operation definition for each hook.
 * A platform overrides only the stages it needs with external strong symbols.
 */
/* Runs before console and shared runtime-state validation. */
void platform_early_init(void);

/* Configures architecture-specific state after runtime validation. */
void platform_arch_init(void);

/* Configures board devices and services required by the current image. */
void platform_init(void);

/* Transfers control or returns when this image has no configured successor. */
void platform_handoff(void);

#endif /* SIMPLE_BOOT_PLATFORM_H */
