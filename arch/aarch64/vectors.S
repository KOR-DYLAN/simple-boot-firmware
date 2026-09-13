/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: arch/aarch64/vectors.S
 * Brief: Define the AArch64 exception table and default handler.
 */

/* AArch64 exception vector table and default handler. */

/* Includes --------------------------------------------------------------- */
#include "asm/asm_macros.S"

/* Assembler configuration ------------------------------------------------ */
.arch armv8-a

/* Default exception handler ---------------------------------------------- */
func default_exception_handler
    unwind_root
    /*
     * Wait without using the stack. Inspect CurrentEL, ESR_ELx, ELR_ELx, FAR_ELx, and
     * SPSR_ELx in GDB.
     */
    msr     daifset, #A64_DAIF_ALL
.Lexception:
    wfi
    b       .Lexception
endfunc default_exception_handler

/* Exception vector table ------------------------------------------------- */
vector_base aarch64_vectors
    /* Current exception level using SP_EL0. */
vector_entry aarch64_vector_current_sp0_sync
    b       default_exception_handler
end_vector_entry aarch64_vector_current_sp0_sync
vector_entry aarch64_vector_current_sp0_irq
    b       default_exception_handler
end_vector_entry aarch64_vector_current_sp0_irq
vector_entry aarch64_vector_current_sp0_fiq
    b       default_exception_handler
end_vector_entry aarch64_vector_current_sp0_fiq
vector_entry aarch64_vector_current_sp0_serror
    b       default_exception_handler
end_vector_entry aarch64_vector_current_sp0_serror

    /* Current exception level using SP_ELx. */
vector_entry aarch64_vector_current_spx_sync
    b       default_exception_handler
end_vector_entry aarch64_vector_current_spx_sync
vector_entry aarch64_vector_current_spx_irq
    b       default_exception_handler
end_vector_entry aarch64_vector_current_spx_irq
vector_entry aarch64_vector_current_spx_fiq
    b       default_exception_handler
end_vector_entry aarch64_vector_current_spx_fiq
vector_entry aarch64_vector_current_spx_serror
    b       default_exception_handler
end_vector_entry aarch64_vector_current_spx_serror

    /* Lower exception level running AArch64. */
vector_entry aarch64_vector_lower_aarch64_sync
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch64_sync
vector_entry aarch64_vector_lower_aarch64_irq
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch64_irq
vector_entry aarch64_vector_lower_aarch64_fiq
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch64_fiq
vector_entry aarch64_vector_lower_aarch64_serror
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch64_serror

    /* Lower exception level running AArch32. */
vector_entry aarch64_vector_lower_aarch32_sync
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch32_sync
vector_entry aarch64_vector_lower_aarch32_irq
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch32_irq
vector_entry aarch64_vector_lower_aarch32_fiq
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch32_fiq
vector_entry aarch64_vector_lower_aarch32_serror
    b       default_exception_handler
end_vector_entry aarch64_vector_lower_aarch32_serror
end_vector_table aarch64_vectors

.section .note.GNU-stack, "", %progbits
