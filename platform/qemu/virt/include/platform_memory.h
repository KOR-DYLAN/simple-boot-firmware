/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: platform/qemu/virt/include/platform_memory.h
 * Brief: Define memory ranges and default region boundaries for virt.
 */

#ifndef QEMU_VIRT_MEMORY_H
#define QEMU_VIRT_MEMORY_H

#include "compiler_rt.h"

/* Platform memory limits ------------------------------------------------- */
/* Allowed code/RO and RW/stack ranges for this QEMU board. */
/* Reserve the first 2 MiB of the 128 MiB RAM for the DTB. */
#define BOARD_CODE_MIN             U(0x40200000)
#define BOARD_CODE_LIMIT           U(0x48000000)
#define BOARD_RAM_MIN              U(0x40000000)
#define BOARD_RAM_LIMIT            U(0x48000000)

/* Bootloader region boundaries ------------------------------------------- */
#ifndef BOOTLOADER1_CODE_START
#define BOOTLOADER1_CODE_START     U(0x40200000)
#endif
#ifndef BOOTLOADER1_RW_START
#define BOOTLOADER1_RW_START       U(0x40400000)
#endif
#ifndef BOOTLOADER1_STACK_END
#define BOOTLOADER1_STACK_END      U(0x407f0000)
#endif
#ifndef BOOTLOADER1_STACK_START
#define BOOTLOADER1_STACK_START    U(0x40800000)
#endif
#ifndef BOOTLOADER2_CODE_START
#define BOOTLOADER2_CODE_START     U(0x41000000)
#endif
#ifndef BOOTLOADER2_RW_START
#define BOOTLOADER2_RW_START       U(0x41200000)
#endif
#ifndef BOOTLOADER2_STACK_END
#define BOOTLOADER2_STACK_END      U(0x415f0000)
#endif
#ifndef BOOTLOADER2_STACK_START
#define BOOTLOADER2_STACK_START    U(0x41600000)
#endif
/* Shared image layout ---------------------------------------------------- */
#include "qemu_image_layout.h"

#endif /* QEMU_VIRT_MEMORY_H */
