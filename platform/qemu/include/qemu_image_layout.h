/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/include/qemu_image_layout.h
 * Brief: Derive one QEMU boot image layout from board memory boundaries.
 */

#ifndef SIMPLE_BOOT_QEMU_IMAGE_LAYOUT_H
#define SIMPLE_BOOT_QEMU_IMAGE_LAYOUT_H

#include "compiler_rt.h"

/* Image selection -------------------------------------------------------- */
#if !defined(PLATFORM_IMAGE) || \
    ((PLATFORM_IMAGE != 1) && (PLATFORM_IMAGE != 2))
#error "PLATFORM_IMAGE must select image 1 or image 2"
#endif

/* Shared region capacities ----------------------------------------------- */
#define PLATFORM_CODE_CAPACITY U(0x00010000)
#define PLATFORM_RO_CAPACITY   U(0x00010000)
#define PLATFORM_RW_CAPACITY   U(0x00010000)

/* Selected image --------------------------------------------------------- */
#ifndef BOOTLOADER2_ENTRY_ADDRESS
#define BOOTLOADER2_ENTRY_ADDRESS BOOTLOADER2_CODE_START
#endif

#if PLATFORM_IMAGE == 2
#define PLATFORM_IMAGE_NAME          "bootloader2"
#define PLATFORM_IMAGE_ENTRY_ADDRESS BOOTLOADER2_ENTRY_ADDRESS
#define PLATFORM_IMAGE_CODE_START    BOOTLOADER2_CODE_START
#define PLATFORM_IMAGE_RW_START      BOOTLOADER2_RW_START
#define PLATFORM_IMAGE_STACK_END     BOOTLOADER2_STACK_END
#define PLATFORM_IMAGE_STACK_START   BOOTLOADER2_STACK_START
#else
#define PLATFORM_IMAGE_NAME          "bootloader1"
#define PLATFORM_IMAGE_CODE_START    BOOTLOADER1_CODE_START
#define PLATFORM_IMAGE_RW_START      BOOTLOADER1_RW_START
#define PLATFORM_IMAGE_STACK_END     BOOTLOADER1_STACK_END
#define PLATFORM_IMAGE_STACK_START   BOOTLOADER1_STACK_START
#if defined(BOARD_RESET_VECTOR_ADDRESS)
#define PLATFORM_IMAGE_RESET_ADDRESS BOARD_RESET_VECTOR_ADDRESS
#endif
#endif

/* Derived region boundaries --------------------------------------------- */
#ifndef CODE_START
#define CODE_START    PLATFORM_IMAGE_CODE_START
#endif
#ifndef CODE_END
#define CODE_END      (CODE_START + PLATFORM_CODE_CAPACITY)
#endif
#ifndef RO_DATA_START
#define RO_DATA_START CODE_END
#endif
#ifndef RO_DATA_END
#define RO_DATA_END   (RO_DATA_START + PLATFORM_RO_CAPACITY)
#endif
#ifndef RW_DATA_START
#define RW_DATA_START PLATFORM_IMAGE_RW_START
#endif
#ifndef RW_DATA_END
#define RW_DATA_END   (RW_DATA_START + PLATFORM_RW_CAPACITY)
#endif
#ifndef STACK_END
#define STACK_END     PLATFORM_IMAGE_STACK_END
#endif
#ifndef STACK_START
#define STACK_START   PLATFORM_IMAGE_STACK_START
#endif

#endif /* SIMPLE_BOOT_QEMU_IMAGE_LAYOUT_H */
