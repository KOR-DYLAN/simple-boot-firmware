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
/**
 * @brief Create an unsigned-long single-bit mask.
 *
 * @param bit Zero-based bit index.
 *
 * @return Mask containing the selected bit.
 */
#define BIT(bit)   (UL(1) << (bit))

/**
 * @brief Create a 32-bit single-bit mask.
 *
 * @param bit Zero-based bit index.
 *
 * @return Mask containing the selected bit.
 */
#define BIT32(bit) (U(1) << (bit))

/**
 * @brief Create a 64-bit single-bit mask.
 *
 * @param bit Zero-based bit index.
 *
 * @return Mask containing the selected bit.
 */
#define BIT64(bit) (ULL(1) << (bit))

/* Contiguous masks ------------------------------------------------------- */
/**
 * @brief Create a contiguous 32-bit mask.
 *
 * @param high Most significant included bit.
 * @param low Least significant included bit.
 *
 * @return Mask with bits high through low set.
 */
#define GENMASK32(high, low) \
    ((~U(0) << (low)) & (~U(0) >> (U(31) - (high))))

/**
 * @brief Create a contiguous 64-bit mask.
 *
 * @param high Most significant included bit.
 * @param low Least significant included bit.
 *
 * @return Mask with bits high through low set.
 */
#define GENMASK64(high, low) \
    ((~ULL(0) << (low)) & (~ULL(0) >> (U(63) - (high))))
#if defined(BOOT_ARCH_AARCH64)
#define GENMASK(high, low) GENMASK64(high, low)
#else
#define GENMASK(high, low) GENMASK32(high, low)
#endif

/* Bit-field operations -------------------------------------------------- */
/* Field masks shall be non-zero and contain one contiguous set-bit range. */
/**
 * @brief Obtain the least significant set bit of a mask.
 *
 * @param mask Non-zero field mask.
 *
 * @return Single-bit mask for the field position.
 */
#define FIELD_LOW_BIT(mask)     ((mask) & (-(mask)))

/**
 * @brief Extract an unshifted field value.
 *
 * @param mask Contiguous field mask.
 * @param value Register or packed value.
 *
 * @return Extracted field value shifted to bit zero.
 */
#define FIELD_GET(mask, value)  (((value) & (mask)) / FIELD_LOW_BIT(mask))

/**
 * @brief Position a value inside a field mask.
 *
 * @param mask Contiguous field mask.
 * @param value Unshifted field value.
 *
 * @return Value shifted and masked for insertion.
 */
#define FIELD_PREP(mask, value) (((value) * FIELD_LOW_BIT(mask)) & (mask))

/**
 * @brief Test whether a value fits within a field mask.
 *
 * @param mask Contiguous field mask.
 * @param value Unshifted field value.
 *
 * @return Non-zero when value fits completely; otherwise zero.
 */
#define FIELD_FIT(mask, value)  \
    (FIELD_PREP(mask, value) == ((value) * FIELD_LOW_BIT(mask)))

#endif /* SIMPLE_BOOT_BIT_H */
