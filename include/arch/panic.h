/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/arch/panic.h
 * Brief: Declare architecture-specific panic register reporting.
 */

#ifndef SIMPLE_BOOT_ARCH_PANIC_H
#define SIMPLE_BOOT_ARCH_PANIC_H

/* Includes --------------------------------------------------------------- */
#include <stdint.h>

/* Panic diagnostics ------------------------------------------------------ */
void arch_panic_dump(uintptr_t program_counter);

#endif /* SIMPLE_BOOT_ARCH_PANIC_H */
