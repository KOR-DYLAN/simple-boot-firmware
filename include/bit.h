/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/bit.h
 * Brief: Define integer bit, mask, and bit-field operations.
 */

#ifndef SIMPLE_BOOT_BIT_H
#define SIMPLE_BOOT_BIT_H

/* Includes --------------------------------------------------------------- */
#include "compiler_rt.h"

/* Single-bit masks ------------------------------------------------------- */
/* The bit index shall be smaller than the width of the selected type. */
#define BIT(bit)   (UL(1) << (bit))
#define BIT32(bit) (U(1) << (bit))
#define BIT64(bit) (ULL(1) << (bit))

/* Contiguous masks ------------------------------------------------------- */
#define GENMASK32(high, low) \
    ((~U(0) << (low)) & (~U(0) >> (U(31) - (high))))
#define GENMASK64(high, low) \
    ((~ULL(0) << (low)) & (~ULL(0) >> (U(63) - (high))))
#if defined(BOOT_ARCH_AARCH64)
#define GENMASK(high, low) GENMASK64(high, low)
#else
#define GENMASK(high, low) GENMASK32(high, low)
#endif

/* Bit-field operations -------------------------------------------------- */
/* Field masks shall be non-zero and contain one contiguous set-bit range. */
#define FIELD_LOW_BIT(mask)     ((mask) & (-(mask)))
#define FIELD_GET(mask, value)  (((value) & (mask)) / FIELD_LOW_BIT(mask))
#define FIELD_PREP(mask, value) (((value) * FIELD_LOW_BIT(mask)) & (mask))
#define FIELD_FIT(mask, value)  \
    (FIELD_PREP(mask, value) == ((value) * FIELD_LOW_BIT(mask)))

#endif /* SIMPLE_BOOT_BIT_H */
