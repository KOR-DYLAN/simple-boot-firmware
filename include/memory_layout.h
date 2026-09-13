/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/memory_layout.h
 * Brief: Select and validate shared platform memory definitions.
 */

#ifndef SIMPLE_BOOT_MEMORY_LAYOUT_H
#define SIMPLE_BOOT_MEMORY_LAYOUT_H

/* Includes --------------------------------------------------------------- */
#include "arch/arch_def.h"

/* Macro definitions ------------------------------------------------------ */
#define MEMORY_REGION_ALIGNMENT  16
#define MEMORY_REGION_ALIGN_MASK (MEMORY_REGION_ALIGNMENT - 1)

/*
 * Shared by C, assembly, and GNU ld; all addresses belong to the platform. Use integer
 * expressions without C suffixes, casts, or sizeof. Ends are exclusive; STACK_START is
 * the initial SP of the descending stack.
 */

/* Platform header selection ---------------------------------------------- */
#ifdef BOOT_MEMORY_CONFIG
#include BOOT_MEMORY_CONFIG
#endif
#include "platform_memory.h"

/* Required platform definitions ------------------------------------------ */
#if !defined(CODE_START) || !defined(CODE_END) || \
    !defined(RO_DATA_START) || !defined(RO_DATA_END) || \
    !defined(RW_DATA_START) || !defined(RW_DATA_END) || \
    !defined(STACK_END) || !defined(STACK_START)
#error "The platform must define all code, RO, RW, and stack boundaries"
#endif

#endif /* SIMPLE_BOOT_MEMORY_LAYOUT_H */
