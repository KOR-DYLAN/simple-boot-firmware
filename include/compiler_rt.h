/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/compiler_rt.h
 * Brief: Normalize compiler-specific attributes and built-in operations.
 */

#ifndef SIMPLE_BOOT_COMPILER_RT_H
#define SIMPLE_BOOT_COMPILER_RT_H

/* Integer literal construction ------------------------------------------- */
#if defined(__ASSEMBLER__) || defined(__LINKER__)
#define U(value)   value
#define L(value)   value
#define UL(value)  value
#define LL(value)  value
#define ULL(value) value
#else
#define U(value)   value##U
#define L(value)   value##L
#define UL(value)  value##UL
#define LL(value)  value##LL
#define ULL(value) value##ULL
#endif

/* C language compatibility ----------------------------------------------- */
#if !defined(__ASSEMBLER__) && !defined(__LINKER__)
#define COMPILER_TOKEN_JOIN_RAW(left, right) left##right
#define COMPILER_TOKEN_JOIN(left, right)     COMPILER_TOKEN_JOIN_RAW(left, right)
#define COMPILER_STATIC_ASSERT(condition, message)                         \
    typedef char COMPILER_TOKEN_JOIN(compiler_static_assertion_, __LINE__) \
        [(condition) ? 1 : -1]
#endif

/* Compiler attributes ---------------------------------------------------- */
#if defined(__GNUC__) || defined(__clang__)
#define COMPILER_ALIGNED(value)          __attribute__((aligned(value)))
#define COMPILER_ALWAYS_INLINE           __attribute__((always_inline)) inline
#define COMPILER_FORMAT_PRINTF(f, a)     __attribute__((format(printf, f, a)))
#define COMPILER_NOINLINE                __attribute__((noinline))
#define COMPILER_NORETURN                __attribute__((noreturn))
#define COMPILER_OFFSETOF(type, member)  __builtin_offsetof(type, member)
#define COMPILER_PACKED                  __attribute__((packed))
#define COMPILER_RESTRICT                __restrict__
#define COMPILER_RETURN_ADDRESS()        __builtin_return_address(0)
#define COMPILER_UNREACHABLE()           __builtin_unreachable()
#define COMPILER_UNUSED                  __attribute__((unused))
#define COMPILER_USED                    __attribute__((used))
#define COMPILER_WEAK                    __attribute__((weak))
#else
#error "Unsupported compiler: add its runtime definitions to compiler_rt.h"
#endif

/* Compiler barriers ------------------------------------------------------ */
#define COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

#endif /* SIMPLE_BOOT_COMPILER_RT_H */
