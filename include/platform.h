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
/**
 * @brief Perform optional initialization before console validation.
 */
void platform_early_init(void);

/**
 * @brief Configure optional architecture state after runtime validation.
 */
void platform_arch_init(void);

/**
 * @brief Configure optional board devices required by the current image.
 */
void platform_init(void);

/**
 * @brief Transfer control, or return when no successor image is configured.
 */
void platform_handoff(void);

#endif /* SIMPLE_BOOT_PLATFORM_H */
