/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/linker_defs.h
 * Brief: Define ELF segment permission flags for the linker script.
 */

#ifndef SIMPLE_BOOT_LINKER_DEFS_H
#define SIMPLE_BOOT_LINKER_DEFS_H

/* Macro definitions ------------------------------------------------------ */
#define ELF_SEGMENT_EXECUTE 1
#define ELF_SEGMENT_WRITE   2
#define ELF_SEGMENT_READ    4

#endif /* SIMPLE_BOOT_LINKER_DEFS_H */
