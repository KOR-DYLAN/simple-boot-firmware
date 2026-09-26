/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/boot.h
 * Brief: Declare boot, image handoff, and linker-defined boundaries.
 */

#ifndef SIMPLE_BOOT_H
#define SIMPLE_BOOT_H

/* Includes --------------------------------------------------------------- */
#include <stdint.h>
#include "compiler_rt.h"

/* Public entry points ---------------------------------------------------- */
/**
 * @brief Enter C boot flow after assembly runtime initialization.
 */
void boot_main(void);

/**
 * @brief Provide a stable debugger breakpoint after runtime validation.
 */
void boot_ready(void);

/**
 * @brief Validate, report, and execute the current boot image flow.
 *
 * @param image_name Null-terminated image name used in diagnostics.
 */
void boot_run(const char *image_name);

/**
 * @brief Report a destination immediately before image handoff.
 *
 * @param name Null-terminated destination image name.
 * @param entry Destination entry address.
 */
void boot_print_handoff(const char *name, uintptr_t entry);

/**
 * @brief Replace the current image context and enter another image.
 *
 * @param entry Destination entry address.
 * @param argument0 First machine-word argument.
 * @param argument1 Second machine-word argument.
 * @param argument2 Third machine-word argument.
 * @param argument3 Fourth machine-word argument.
 */
void boot_jump(uintptr_t entry, uintptr_t argument0, uintptr_t argument1,
               uintptr_t argument2, uintptr_t argument3)
    COMPILER_NORETURN;

/**
 * @brief Stop execution after masking supported interrupt sources.
 */
void boot_halt(void) COMPILER_NORETURN;

/* Linker symbols --------------------------------------------------------- */
/* All end symbols represent exclusive addresses. */
extern unsigned char __code_start[], __code_end[];
extern unsigned char __rodata_start[], __rodata_end[], __ro_image_end[];
extern unsigned char __data_load_start[], __data_start[], __data_end[];
extern unsigned char __bss_start[], __bss_end[];
extern unsigned char __stack_bottom[], __stack_top[];

#endif /* SIMPLE_BOOT_H */
