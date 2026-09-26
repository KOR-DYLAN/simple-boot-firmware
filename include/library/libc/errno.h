/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: include/library/libc/errno.h
 * Brief: Define error numbers used by freestanding firmware services.
 */

#ifndef SIMPLE_BOOT_ERRNO_H
#define SIMPLE_BOOT_ERRNO_H

/* Error codes ------------------------------------------------------------ */
/* Keep these ABI-compatible error numbers as unsuffixed int constants. */
#define EPERM        1   /* Operation not permitted. */
#define ENOENT       2   /* Entry or file not found. */
#define EINTR        4   /* Operation interrupted. */
#define EIO          5   /* Input or output failure. */
#define ENXIO        6   /* Device or address not available. */
#define E2BIG        7   /* Argument list is too large. */
#define ENOEXEC      8   /* Executable format is invalid. */
#define EBADF        9   /* File descriptor is invalid. */
#define ECHILD       10  /* Child process does not exist. */
#define EAGAIN       11  /* Resource is temporarily unavailable. */
#define ENOMEM       12  /* Memory is insufficient. */
#define EACCES       13  /* Access is denied. */
#define EFAULT       14  /* Memory address is invalid. */
#define EBUSY        16  /* Resource or device is busy. */
#define EEXIST       17  /* Entry or file already exists. */
#define EXDEV        18  /* Cross-device operation is invalid. */
#define ENODEV       19  /* Device is not available. */
#define ENOTDIR      20  /* Path component is not a directory. */
#define EISDIR       21  /* Operation requires a non-directory. */
#define EINVAL       22  /* Argument is invalid. */
#define ENFILE       23  /* System file table is full. */
#define EMFILE       24  /* Open-file limit has been reached. */
#define ENOTTY       25  /* Device control request is unsupported. */
#define EFBIG        27  /* File is too large. */
#define ENOSPC       28  /* Storage space is exhausted. */
#define ESPIPE       29  /* Seek is invalid for this object. */
#define EROFS        30  /* File system is read-only. */
#define EMLINK       31  /* Link count limit has been reached. */
#define EPIPE        32  /* Communication endpoint is closed. */
#define EDOM         33  /* Argument is outside the function domain. */
#define ERANGE       34  /* Result is outside the representable range. */
#define ENAMETOOLONG 36  /* Name or path is too long. */
#define ENOLCK       37  /* Lock resource is unavailable. */
#define ENOSYS       38  /* Function is not implemented. */
#define ENOTEMPTY    39  /* Directory is not empty. */
#define ELOOP        40  /* Too many symbolic links were followed. */
#define ENOMSG       42  /* Requested message is unavailable. */
#define EIDRM        43  /* IPC object has been removed. */
#define EPROTO       71  /* Protocol operation failed. */
#define EBADMSG      74  /* Message data is invalid. */
#define EOVERFLOW    75  /* Value is too large for its destination. */
#define EILSEQ       84  /* Character sequence is invalid. */
#define ENOTSUP      95  /* Operation is not supported. */
#define ETIMEDOUT    110 /* Operation timed out. */
#define ECANCELED    125 /* Operation was canceled. */

/* Equivalent retry conditions share one error number. */
#define EWOULDBLOCK EAGAIN /* Operation would block. */

/**
 * @brief Error code produced by the most recent failing libc operation.
 */
extern int errno;

#endif /* SIMPLE_BOOT_ERRNO_H */
