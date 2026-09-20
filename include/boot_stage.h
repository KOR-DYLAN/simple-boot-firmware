/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/boot_stage.h
 * Brief: Define boot stage identifiers and payload handoff defaults.
 */

#ifndef SIMPLE_BOOT_STAGE_H
#define SIMPLE_BOOT_STAGE_H

/* Boot stage identifiers ------------------------------------------------- */
#define BOOT_STAGE_1 1
#define BOOT_STAGE_2 2
#define BOOT_HANDOFF_ARGUMENT_NONE 0

#ifndef BOOT_STAGE
#define BOOT_STAGE BOOT_STAGE_1
#endif

#if BOOT_STAGE != BOOT_STAGE_1 && BOOT_STAGE != BOOT_STAGE_2
#error "BOOT_STAGE must select bootloader1 or bootloader2"
#endif

#endif /* SIMPLE_BOOT_STAGE_H */
