/*
 * Copyright (c) 2026 simple-boot contributors
 * SPDX-License-Identifier: MIT
 *
 * File: boot/boot.ld.S
 * Brief: Place one boot stage and validate its platform memory map.
 */

/* Includes --------------------------------------------------------------- */
#include "memory_layout.h"
#include "linker_defs.h"

/* Entry point ------------------------------------------------------------ */
ENTRY(_start)

/* Memory regions --------------------------------------------------------- */
MEMORY
{
    CODE  (rx) : ORIGIN = CODE_START, LENGTH = CODE_END - CODE_START
    RO    (r)  : ORIGIN = RO_DATA_START, LENGTH = RO_DATA_END - RO_DATA_START
    RW    (rw) : ORIGIN = RW_DATA_START, LENGTH = RW_DATA_END - RW_DATA_START
    STACK (rw) : ORIGIN = STACK_END, LENGTH = STACK_START - STACK_END
}

/* Region symbols for build-time reports ----------------------------------- */
__region_code_start = CODE_START;
__region_code_end = CODE_END;
__region_ro_start = RO_DATA_START;
__region_ro_end = RO_DATA_END;
__region_rw_start = RW_DATA_START;
__region_rw_end = RW_DATA_END;

/* Program headers -------------------------------------------------------- */
/*
 * Load .data initializers into RO, then copy them to RW at runtime. NOLOAD sections
 * add no file bytes to the stage binary; reserved gaps are excluded from runtime
 * initialization.
 */
PHDRS
{
    code PT_LOAD FLAGS(ELF_SEGMENT_READ | ELF_SEGMENT_EXECUTE);
    ro PT_LOAD FLAGS(ELF_SEGMENT_READ);
    data PT_LOAD FLAGS(ELF_SEGMENT_READ | ELF_SEGMENT_WRITE);
    bss PT_LOAD FLAGS(ELF_SEGMENT_READ | ELF_SEGMENT_WRITE);
}

/* Output sections -------------------------------------------------------- */
SECTIONS
{
    .text CODE_START :
    {
        __code_start = .;
#if defined(BOOT_ARCH_AARCH64)
        KEEP(*(.text.boot))
        KEEP(*(.vectors))
#else
        KEEP(*(.vectors))
        KEEP(*(.text.boot))
#endif
        *(.text .text.*)
        . = ALIGN(MEMORY_REGION_ALIGNMENT);
        __code_end = .;
    } > CODE :code

    .rodata RO_DATA_START :
    {
        __rodata_start = .;
        *(.rodata .rodata.*)
        . = ALIGN(MEMORY_REGION_ALIGNMENT);
        __rodata_end = .;
    } > RO :ro

    .data RW_DATA_START : AT(__rodata_end)
    {
        __data_start = .;
        *(.data .data.*)
        . = ALIGN(MEMORY_REGION_ALIGNMENT);
        __data_end = .;
    } > RW :data
    __data_load_start = LOADADDR(.data);
    __ro_image_end = LOADADDR(.data) + SIZEOF(.data);

    .bss (NOLOAD) : AT(ADDR(.bss))
    {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(MEMORY_REGION_ALIGNMENT);
        __bss_end = .;
    } > RW :bss

    .stack STACK_END (NOLOAD) :
    {
        __stack_bottom = .;
        . += STACK_START - STACK_END;
        __stack_top = .;
    } > STACK :NONE

    /* Discard metadata and runtime unwind tables from the load image. */
    /DISCARD/ :
    {
        *(.comment .note .note.* .eh_frame .eh_frame.*)
        *(.ARM.exidx* .ARM.extab*)
    }

/* Memory layout assertions ----------------------------------------------- */
    ASSERT(CODE_START < CODE_END, "CODE region must be non-empty")
    ASSERT(CODE_END == RO_DATA_START, "CODE_END must equal RO_DATA_START")
    ASSERT(RO_DATA_START < RO_DATA_END, "RO region must be non-empty")
    ASSERT(RO_DATA_END <= RW_DATA_START, "RO and RW regions overlap")
    ASSERT(RW_DATA_START < RW_DATA_END, "RW region must be non-empty")
    ASSERT(RW_DATA_END <= STACK_END, "RW and stack regions overlap")
    ASSERT(STACK_END < STACK_START, "Descending stack requires STACK_END < STACK_START")
    ASSERT(((CODE_START | CODE_END | RO_DATA_START | RO_DATA_END |
             RW_DATA_START | RW_DATA_END | STACK_END | STACK_START) &
            MEMORY_REGION_ALIGN_MASK) == 0,
           "Memory boundaries must satisfy the configured region alignment")
    ASSERT(__code_end <= CODE_END, "Code exceeds CODE_END")
    ASSERT(__ro_image_end <= RO_DATA_END, "RO data and .data initializers exceed RO_DATA_END")
    ASSERT(__bss_end <= RW_DATA_END, "RW .data and .bss exceed RW_DATA_END")
    ASSERT(CODE_START >= BOARD_CODE_MIN && RO_DATA_END <= BOARD_CODE_LIMIT,
           "Code/RO outside the selected platform memory")
    ASSERT(RW_DATA_START >= BOARD_RAM_MIN && STACK_START <= BOARD_RAM_LIMIT,
           "RW/stack outside the selected platform RAM")
#if BOOT_STAGE == BOOT_STAGE_1 && defined(BOARD_RESET_VECTOR_ADDRESS)
    ASSERT(CODE_START == BOARD_RESET_VECTOR_ADDRESS,
           "Platform reset vector address must be preserved")
#endif
#if BOOT_STAGE == BOOT_STAGE_2
    ASSERT(CODE_START == BOOTLOADER2_ENTRY_ADDRESS,
           "Bootloader2 entry must equal its code start")
#endif
    ASSERT((CODE_START & (ARCH_VECTOR_ALIGNMENT - 1)) == 0,
           "Code start must satisfy the architecture vector alignment")
}
