/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/boot.h
 * Brief: Declare boot entry points and linker-defined memory boundaries.
 */

#ifndef SIMPLE_BOOT_H
#define SIMPLE_BOOT_H

/* Includes --------------------------------------------------------------- */
#include "stdint.h"

/* Public entry points ---------------------------------------------------- */
void boot_main(void);
void boot_ready(void);
void boot_halt(void) __attribute__((noreturn));

/* Linker symbols --------------------------------------------------------- */
extern unsigned char __code_start[], __code_end[];
extern unsigned char __rodata_start[], __rodata_end[], __ro_image_end[];
extern unsigned char __data_load_start[], __data_start[], __data_end[];
extern unsigned char __bss_start[], __bss_end[];
extern unsigned char __stack_bottom[], __stack_top[];

#endif /* SIMPLE_BOOT_H */
