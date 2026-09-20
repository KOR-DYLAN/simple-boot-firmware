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

/* Bootloader region boundaries ------------------------------------------- */
#ifndef BOOTLOADER1_CODE_START
#define BOOTLOADER1_CODE_START BOARD_RESET_VECTOR_ADDRESS
#endif
#ifndef BOOTLOADER1_RW_START
#define BOOTLOADER1_RW_START BOARD_RAM_MIN
#endif
#ifndef BOOTLOADER1_STACK_END
#define BOOTLOADER1_STACK_END 0x20030000
#endif
#ifndef BOOTLOADER1_STACK_START
#define BOOTLOADER1_STACK_START 0x20040000
#endif
#ifndef BOOTLOADER2_CODE_START
#define BOOTLOADER2_CODE_START 0x00100000
#endif
#ifndef BOOTLOADER2_RW_START
#define BOOTLOADER2_RW_START 0x20100000
#endif
#ifndef BOOTLOADER2_STACK_END
#define BOOTLOADER2_STACK_END 0x20130000
#endif
#ifndef BOOTLOADER2_STACK_START
#define BOOTLOADER2_STACK_START 0x20140000
#endif
#ifndef BOOTLOADER2_ENTRY_ADDRESS
#define BOOTLOADER2_ENTRY_ADDRESS BOOTLOADER2_CODE_START
#endif

/* Selected stage boundaries --------------------------------------------- */
#if BOOT_STAGE == BOOT_STAGE_2
#define PLATFORM_STAGE_CODE_START  BOOTLOADER2_CODE_START
#define PLATFORM_STAGE_RW_START    BOOTLOADER2_RW_START
#define PLATFORM_STAGE_STACK_END   BOOTLOADER2_STACK_END
#define PLATFORM_STAGE_STACK_START BOOTLOADER2_STACK_START
#else
#define PLATFORM_STAGE_CODE_START  BOOTLOADER1_CODE_START
#define PLATFORM_STAGE_RW_START    BOOTLOADER1_RW_START
#define PLATFORM_STAGE_STACK_END   BOOTLOADER1_STACK_END
#define PLATFORM_STAGE_STACK_START BOOTLOADER1_STACK_START
#endif

/* Default region boundaries --------------------------------------------- */
#ifndef CODE_START
#define CODE_START PLATFORM_STAGE_CODE_START
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
#define RW_DATA_START PLATFORM_STAGE_RW_START
#endif

#ifndef RW_DATA_END
#define RW_DATA_END (RW_DATA_START + PLATFORM_RW_CAPACITY)
#endif

#ifndef STACK_END
#define STACK_END PLATFORM_STAGE_STACK_END
#endif

#ifndef STACK_START
#define STACK_START PLATFORM_STAGE_STACK_START
#endif

#endif /* QEMU_MPS2_AN385_MEMORY_H */
