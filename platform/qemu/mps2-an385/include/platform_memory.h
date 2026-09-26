/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/mps2-an385/include/platform_memory.h
 * Brief: Define memory ranges and default region boundaries for mps2-an385.
 */

#ifndef QEMU_MPS2_AN385_MEMORY_H
#define QEMU_MPS2_AN385_MEMORY_H

#include "compiler_rt.h"

/* Platform memory limits ------------------------------------------------- */
/* Allowed code/RO and RW/stack ranges for this QEMU board. */
/* Code uses ZBT SSRAM1; RW uses SSRAM2 and SSRAM3. Reset vectors start at 0. */
#define BOARD_RESET_VECTOR_ADDRESS U(0x00000000)
#define BOARD_CODE_MIN             U(0x00000000)
#define BOARD_CODE_LIMIT           U(0x00400000)
#define BOARD_RAM_MIN              U(0x20000000)
#define BOARD_RAM_LIMIT            U(0x20400000)

/* Bootloader region boundaries ------------------------------------------- */
#ifndef BOOTLOADER1_CODE_START
#define BOOTLOADER1_CODE_START     BOARD_RESET_VECTOR_ADDRESS
#endif
#ifndef BOOTLOADER1_RW_START
#define BOOTLOADER1_RW_START       BOARD_RAM_MIN
#endif
#ifndef BOOTLOADER1_STACK_END
#define BOOTLOADER1_STACK_END      U(0x20030000)
#endif
#ifndef BOOTLOADER1_STACK_START
#define BOOTLOADER1_STACK_START    U(0x20040000)
#endif
#ifndef BOOTLOADER2_CODE_START
#define BOOTLOADER2_CODE_START     U(0x00100000)
#endif
#ifndef BOOTLOADER2_RW_START
#define BOOTLOADER2_RW_START       U(0x20100000)
#endif
#ifndef BOOTLOADER2_STACK_END
#define BOOTLOADER2_STACK_END      U(0x20130000)
#endif
#ifndef BOOTLOADER2_STACK_START
#define BOOTLOADER2_STACK_START    U(0x20140000)
#endif
/* Shared image layout ---------------------------------------------------- */
#include "qemu_image_layout.h"

#endif /* QEMU_MPS2_AN385_MEMORY_H */
