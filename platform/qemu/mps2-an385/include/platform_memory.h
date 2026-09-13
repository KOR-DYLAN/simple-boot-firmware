/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/mps2-an385/include/platform_memory.h
 * Brief: Define memory ranges and default region boundaries for mps2-an385.
 */

#ifndef QEMU_MPS2_AN385_MEMORY_H
#define QEMU_MPS2_AN385_MEMORY_H

/* Platform memory limits ------------------------------------------------- */
/* Allowed code/RO and RW/stack ranges for this QEMU board. */
/* Code uses ZBT SSRAM1; RW uses SSRAM2 and SSRAM3. Reset vectors start at 0. */
#define BOARD_RESET_VECTOR_ADDRESS 0x00000000
#define BOARD_CODE_MIN             0x00000000
#define BOARD_CODE_LIMIT           0x00400000
#define BOARD_RAM_MIN              0x20000000
#define BOARD_RAM_LIMIT            0x20400000

/* Region capacity macros ------------------------------------------------- */
#define PLATFORM_CODE_CAPACITY 0x00010000
#define PLATFORM_RO_CAPACITY   0x00010000
#define PLATFORM_RW_CAPACITY   0x00010000

/* Default region boundaries ---------------------------------------------- */
#ifndef CODE_START
#define CODE_START 0x00000000
#endif

#ifndef CODE_END
#define CODE_END (CODE_START + PLATFORM_CODE_CAPACITY)
#endif

#ifndef RO_DATA_START
#define RO_DATA_START CODE_END
#endif

#ifndef RO_DATA_END
#define RO_DATA_END (RO_DATA_START + PLATFORM_RO_CAPACITY)
#endif

#ifndef RW_DATA_START
#define RW_DATA_START 0x20000000
#endif

#ifndef RW_DATA_END
#define RW_DATA_END (RW_DATA_START + PLATFORM_RW_CAPACITY)
#endif

#ifndef STACK_END
#define STACK_END 0x20030000
#endif

#ifndef STACK_START
#define STACK_START 0x20040000
#endif

#endif /* QEMU_MPS2_AN385_MEMORY_H */
