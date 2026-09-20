/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/virt/include/platform_memory.h
 * Brief: Define memory ranges and default region boundaries for virt.
 */

#ifndef QEMU_VIRT_MEMORY_H
#define QEMU_VIRT_MEMORY_H

/* Platform memory limits ------------------------------------------------- */
/* Allowed code/RO and RW/stack ranges for this QEMU board. */
/* Reserve the first 2 MiB of the 128 MiB RAM for the DTB. */
#define BOARD_CODE_MIN   0x40200000
#define BOARD_CODE_LIMIT 0x48000000
#define BOARD_RAM_MIN    0x40000000
#define BOARD_RAM_LIMIT  0x48000000

/* Region capacity macros ------------------------------------------------- */
#define PLATFORM_CODE_CAPACITY 0x00010000
#define PLATFORM_RO_CAPACITY   0x00010000
#define PLATFORM_RW_CAPACITY   0x00010000

/* Bootloader region boundaries ------------------------------------------- */
#ifndef BOOTLOADER1_CODE_START
#define BOOTLOADER1_CODE_START 0x40200000
#endif
#ifndef BOOTLOADER1_RW_START
#define BOOTLOADER1_RW_START 0x40400000
#endif
#ifndef BOOTLOADER1_STACK_END
#define BOOTLOADER1_STACK_END 0x407f0000
#endif
#ifndef BOOTLOADER1_STACK_START
#define BOOTLOADER1_STACK_START 0x40800000
#endif
#ifndef BOOTLOADER2_CODE_START
#define BOOTLOADER2_CODE_START 0x41000000
#endif
#ifndef BOOTLOADER2_RW_START
#define BOOTLOADER2_RW_START 0x41200000
#endif
#ifndef BOOTLOADER2_STACK_END
#define BOOTLOADER2_STACK_END 0x415f0000
#endif
#ifndef BOOTLOADER2_STACK_START
#define BOOTLOADER2_STACK_START 0x41600000
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

#endif /* QEMU_VIRT_MEMORY_H */
