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
/* Assembly startup enters boot_main after runtime memory initialization. */
void boot_main(void);

/* Stable debugger breakpoint reached after runtime validation succeeds. */
void boot_ready(void);

/* Validate and report the current image; failures halt instead of returning. */
void boot_run(const char *image_name);

/* Report the destination immediately before a platform handoff. */
void boot_print_handoff(const char *name, uintptr_t entry);

/* Replace the current image context and enter the destination image. */
void boot_jump(uintptr_t entry, uintptr_t argument0, uintptr_t argument1,
               uintptr_t argument2, uintptr_t argument3)
    COMPILER_NORETURN;

/* Stop execution with interrupts masked where the architecture supports it. */
void boot_halt(void) COMPILER_NORETURN;

/* Linker symbols --------------------------------------------------------- */
/* All end symbols represent exclusive addresses. */
extern unsigned char __code_start[], __code_end[];
extern unsigned char __rodata_start[], __rodata_end[], __ro_image_end[];
extern unsigned char __data_load_start[], __data_start[], __data_end[];
extern unsigned char __bss_start[], __bss_end[];
extern unsigned char __stack_bottom[], __stack_top[];

#endif /* SIMPLE_BOOT_H */
