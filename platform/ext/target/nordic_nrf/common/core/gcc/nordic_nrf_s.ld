;/*
; * Copyright (c) 2009-2022 Arm Limited
; * Copyright (c) 2020 Nordic Semiconductor ASA
; * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
; * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 gcc_arm.ld
; */

/* Linker script to configure memory regions. */
/* This file will be run trough the pre-processor. */

#include "region_defs.h"

MEMORY
{
  FLASH    (rx)  : ORIGIN = S_CODE_START, LENGTH = S_CODE_SIZE
  RAM      (rwx) : ORIGIN = S_DATA_START, LENGTH = S_DATA_SIZE
#if defined(S_CODE_SRAM_ALIAS_BASE)
  CODE_RAM (rwx) : ORIGIN = S_CODE_SRAM_ALIAS_BASE, LENGTH = TOTAL_CODE_SRAM_SIZE
#endif
#if defined(PSA_PROXY_SHARED_MEMORY_BASE)
  PSA_PROXY_SHARED_MEMORY_RAM (rw) : ORIGIN = PSA_PROXY_SHARED_MEMORY_BASE, LENGTH = PSA_PROXY_SHARED_MEMORY_SIZE
#endif
}

#define VENEERS() \
    /* \
     * We need a dummy section to "reset" the location counter to FLASH if the \
     * previous section was > RAM. Not sure why. This avoids the error \
     * "ERROR: CMSE stub ... too far ... from destination" Make a dummy  \
     * assignment so the section is not optimized away. \
     */ \
    .sg_start_dummy : ALIGN(4) \
    { \
        SG_START_DUMMY_UNUSED = .; \
    } > FLASH \
    /* \
     * Place the CMSE Veneers (containing the SG instructions) in a correctly \
     * aligned region so that the SAU can programmed to just set this region as \
     * Non-Secure Callable. \
     */ \
    .gnu.sgstubs CMSE_VENEER_REGION_START_ALIGN : \
    { \
        Load$$LR$$LR_VENEER$$Base = ABSOLUTE(.); \
        *(.gnu.sgstubs*) \
    } > FLASH \
 \
    /* \
     * This is needed because the linker seems to place the veneers *after* the \
     * .gnu.sgstubs output section, so any end alignment applied inside it are \
     * placed before the veneers instead of after. Also, the end alignment seems \
     * to be ignored if not done in an output section, so this cannot be placed \
     * by itself after the .gnu.sgstubs section. \
     */ \
    .sg_end_dummy : ALIGN(32) \
    { \
        . = CMSE_VENEER_REGION_END_ALIGN; \
        Load$$LR$$LR_VENEER$$Limit = ABSOLUTE(.); \
    } > FLASH \
 \
    ASSERT ((Load$$LR$$LR_VENEER$$Limit - Load$$LR$$LR_VENEER$$Base) \
            <= CMSE_VENEER_REGION_SIZE, "Veneer region overflowed")

/* For builds without BL2, or when BL2 will be booting a single
 * combined S and NS image, we force placing the veneers section
 * at the end of the image (position 3), so as not to waste space
 * as a result of the nRF veneer section alignment requirements.
 * For regular builds with BL2 we keep the veneer placement as is
 * (positions 1 or 2 depending on whether PSA_API_TEST_NS is
 * defined).
 */
#if !defined(BL2) || (MCUBOOT_IMAGE_NUMBER == 1)
#define VENEER_POS 3
#elif defined(PSA_API_TEST_NS)
#define VENEER_POS 1
#else
#define VENEER_POS 2
#endif

__msp_stack_size__ = S_MSP_STACK_SIZE;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a libc_nano.a)

ENTRY(Reset_Handler)

SECTIONS
{
    .TFM_VECTORS : ALIGN(4)
    {
        __vectors_start__ = .;
        KEEP(*(.vectors))
        . = ALIGN(4);
        __vectors_end__ = .;

        /* If this image is placed at the beginning of flash make sure we
         * don't put any code in the first 256 bytes of flash as that area might
         * be used for null-pointer dereference detection. It is believed that on
         * nordic platforms the __Vectors_Size will always be greater than 256,
         * but we (potentially) adjust the location pointer anyway just to be safe. */
#if S_CODE_START == 0
        . = MAX(., 256);
#endif

        *startup*(.text*)
        . = ALIGN(4);
    } > FLASH

    .copy.table : ALIGN(4)
    {
        __copy_table_start__ = .;
#ifdef RAM_VECTORS_SUPPORT
        /* Copy interrupt vectors from flash to RAM */
        LONG (__vectors_start__)                            /* From */
        LONG (__ram_vectors_start__)                        /* To   */
        LONG ((__vectors_end__ - __vectors_start__) / 4)    /* Size */
#endif
        LONG (LOADADDR(.TFM_DATA))
        LONG (ADDR(.TFM_DATA))
        LONG (SIZEOF(.TFM_DATA) / 4)

        LONG (LOADADDR(.TFM_PSA_ROT_LINKER_DATA))
        LONG (ADDR(.TFM_PSA_ROT_LINKER_DATA))
        LONG (SIZEOF(.TFM_PSA_ROT_LINKER_DATA) / 4)

        LONG (LOADADDR(.TFM_APP_ROT_LINKER_DATA))
        LONG (ADDR(.TFM_APP_ROT_LINKER_DATA))
        LONG (SIZEOF(.TFM_APP_ROT_LINKER_DATA) / 4)

#if defined (S_RAM_CODE_START)
        LONG (LOADADDR(.TFM_RAM_CODE))
        LONG (ADDR(.TFM_RAM_CODE))
        LONG (SIZEOF(.TFM_RAM_CODE) / 4)
#endif
#if defined(S_CODE_SRAM_ALIAS_BASE)
        LONG (LOADADDR(.ER_CODE_SRAM))
        LONG (ADDR(.ER_CODE_SRAM))
        LONG (SIZEOF(.ER_CODE_SRAM) / 4)
#endif
        __copy_table_end__ = .;
    } > FLASH

    .zero.table : ALIGN(4)
    {
        __zero_table_start__ = .;
        LONG (ADDR(.TFM_BSS))
        LONG (SIZEOF(.TFM_BSS) / 4)
        LONG (ADDR(.TFM_PSA_ROT_LINKER_BSS))
        LONG (SIZEOF(.TFM_PSA_ROT_LINKER_BSS) / 4)

        LONG (ADDR(.TFM_APP_ROT_LINKER_BSS))
        LONG (SIZEOF(.TFM_APP_ROT_LINKER_BSS) / 4)
#if defined(PSA_PROXY_SHARED_MEMORY_BASE)
        LONG (PSA_PROXY_SHARED_MEMORY_BASE)
        LONG (PSA_PROXY_SHARED_MEMORY_SIZE / 4)
#endif
#if defined(CONFIG_TFM_PARTITION_META)
        LONG (ADDR(.TFM_SP_META_PTR))
        LONG (SIZEOF(.TFM_SP_META_PTR) / 4)
#endif
        __zero_table_end__ = .;
    } > FLASH

    .TFM_UNPRIV_CODE : ALIGN(32)
    {
        *libc_nano*:*(.text*)
        *libc_nano*:*(.rodata*)
        *(SFN)
        *libgcc*:*(.text*)
        *libgcc*:*(.rodata*)
        *libtfm_sprt*:*(.text*)
        *libtfm_sprt*:*(.rodata*)
        *psa_client.*(.text*)              /* NXP */
        *psa_client.*(.rodata*)
        *psa_service.*(.text*)             /* NXP */
        *psa_service.*(.rodata*)
        *psa_lifecycle.*(.text*)           /* NXP */
        *psa_lifecycle.*(.rodata*)
        *tfm_log_raw.*(.text*)             /* NXP */
        *tfm_log_raw.*(.rodata*)
        *tfm_psa_call_pack.*(.text*)
        *tfm_psa_call_pack.*(.rodata*)
        *psa_interface_svc.*(.text*)
        *psa_interface_svc.*(.rodata*)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.TFM_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.TFM_UNPRIV_CODE) + SIZEOF(.TFM_UNPRIV_CODE);

    /**** Section for holding partition RO load data */
    /*
     * Sort the partition info by priority to guarantee the initing order.
     * The first loaded partition will be inited at last in SFN model.
     */
    .TFM_SP_LOAD_LIST : ALIGN(4)
    {
       KEEP(*(.part_load_priority_lowest))
       KEEP(*(.part_load_priority_low))
       KEEP(*(.part_load_priority_normal))
       KEEP(*(.part_load_priority_high))
    } > FLASH
    Image$$TFM_SP_LOAD_LIST$$RO$$Base = ADDR(.TFM_SP_LOAD_LIST);
    Image$$TFM_SP_LOAD_LIST$$RO$$Limit = ADDR(.TFM_SP_LOAD_LIST) + SIZEOF(.TFM_SP_LOAD_LIST);
    . = ALIGN(32);

    /**** PSA RoT RO part (CODE + RODATA) start here */
    Image$$TFM_PSA_CODE_START$$Base = .;

    .TFM_PSA_ROT_LINKER : ALIGN(32)
    {
        *tfm_psa_rot_partition*:*(.text*)
        *tfm_psa_rot_partition*:*(.rodata*)
        *(TFM_*_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH

    Image$$TFM_PSA_ROT_LINKER$$RO$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$RO$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);

    /**** PSA RoT RO part (CODE + RODATA) end here */
    Image$$TFM_PSA_CODE_END$$Base = .;

    /**** APPLICATION RoT RO part (CODE + RODATA) start here */
    Image$$TFM_APP_CODE_START$$Base = .;

    .TFM_APP_ROT_LINKER : ALIGN(32)
    {
        *tfm_app_rot_partition*:*(.text*)
        *tfm_app_rot_partition*:*(.rodata*)
        *(TFM_*_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH

    Image$$TFM_APP_ROT_LINKER$$RO$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$RO$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);

    /**** APPLICATION RoT RO part (CODE + RODATA) end here */
    Image$$TFM_APP_CODE_END$$Base = .;

#if defined(S_CODE_SRAM_ALIAS_BASE)
    .ER_CODE_SRAM : ALIGN(4)
    {
        *Driver_GFC100_EFlash.o(.text*)
        *Driver_GFC100_EFlash.o(.rodata*)
        *gfc100_eflash_drv.o(.text*)
        *gfc100_eflash_drv.o(.rodata*)
        *musca_b1_eflash_drv.o(.text*)
        *musca_b1_eflash_drv.o(.rodata*)
        . = ALIGN(4); /* This alignment is needed to make the section size 4 bytes aligned */
    } > CODE_RAM AT > FLASH
    Image$$ER_CODE_SRAM$$RO$$Base = ADDR(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$RO$$Limit = ADDR(.ER_CODE_SRAM) + SIZEOF(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$Base = ADDR(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$Limit = ADDR(.ER_CODE_SRAM) + SIZEOF(.ER_CODE_SRAM);
#endif

#if TFM_LVL != 1
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

#endif /* TFM_LVL != 1 */

#if VENEER_POS == 1
VENEERS()
#endif

    .ER_TFM_CODE : ALIGN(4)
    {
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))


        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
         *crtbegin.o(.dtors)
         *crtbegin?.o(.dtors)
         *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
         *(SORT(.dtors.*))
         *(.dtors)

    } > FLASH

#if VENEER_POS == 2
VENEERS()
#endif

    .tfm_rodata : ALIGN(4)
    {
        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    /**** Base address of secure data area */
    .tfm_secure_data_start :
    {
        . = ABSOLUTE(S_DATA_START) ;
    } > RAM

#ifdef CODE_SHARING
    /* The code sharing between bootloader and runtime requires to share the
     * global variables.
     */
    .TFM_SHARED_SYMBOLS : ALIGN(32)
    {
        . += SHARED_SYMBOL_AREA_SIZE;
    } > RAM
#endif

    /* shared_data and msp_stack are overlapping on purpose when
     * msp_stack is extended until the beginning of RAM, when shared_date
     * was read out by partitions
     */
    .tfm_bl2_shared_data : ALIGN(32)
    {
        . += BOOT_TFM_SHARED_DATA_SIZE;
    } > RAM

    .msp_stack : ALIGN(32)
    {
        . += __msp_stack_size__ - 0x8;
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    .msp_stack_seal_res :
    {
        . += 0x8;
    } > RAM
    __StackSeal = ADDR(.msp_stack_seal_res);

#if defined(ENABLE_HEAP)
    __heap_size__ = S_HEAP_SIZE;
    .heap : ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM
#endif /* defined(ENABLE_HEAP) */

#if defined(CONFIG_TFM_PARTITION_META)
    .TFM_SP_META_PTR : ALIGN(32)
    {
        *(.bss.SP_META_PTR_SPRTL_INST)
        . = ALIGN(32);
    } > RAM
    Image$$TFM_SP_META_PTR$$ZI$$Base = ADDR(.TFM_SP_META_PTR);
    Image$$TFM_SP_META_PTR$$ZI$$Limit = ADDR(.TFM_SP_META_PTR) + SIZEOF(.TFM_SP_META_PTR);
#endif

    /**** APPLICATION RoT DATA start here */
    . = ALIGN(32);
    Image$$TFM_APP_RW_STACK_START$$Base = .;

    .TFM_APP_ROT_LINKER_DATA : ALIGN(32)
    {
        *tfm_app_rot_partition*:*(.data*)
        *(TFM_*_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_APP_ROT_LINKER_DATA$$RW$$Base = ADDR(.TFM_APP_ROT_LINKER_DATA);
    Image$$TFM_APP_ROT_LINKER_DATA$$RW$$Limit = ADDR(.TFM_APP_ROT_LINKER_DATA) + SIZEOF(.TFM_APP_ROT_LINKER_DATA);

    .TFM_APP_ROT_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_APP_ROT_LINKER = .;
        *tfm_app_rot_partition*:*(.bss*)
        *tfm_app_rot_partition*:*(COMMON)
        *(TFM_*_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_APP_ROT_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_APP_ROT_LINKER_BSS);
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_APP_ROT_LINKER_BSS) + SIZEOF(.TFM_APP_ROT_LINKER_BSS);

    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base = .;

#if TFM_LVL != 1
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA : ALIGN(32)
    {
        *libc_nano*:*(.data*)
        . = ALIGN(32);
    } > RAM AT> FLASH

    .TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS : ALIGN(32)
    {
        /* FixMe:
         * Test framework use printf to print out test result. Implementation of
         * printf in GCC libc use static data and heap as well. To be able to
         * execute test suites with TFM_LVL=3 this workaround is needed to
         * allocate libc static data and heap within the data section of secure
         * test partition. This can be removed if test service will be executed
         * in privileged mode.
         */
        *libc_nano*:*(.bss*)
        *libc_nano*:*(COMMON)

        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */

        . = ALIGN(32);
    } > RAM AT> RAM
#else /* TFM_PARTITION_TEST_SECURE_SERVICES */
    .heap : ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM AT> RAM
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#endif /* TFM_LVL != 1 */

    /**** PSA RoT DATA start here */

    Image$$TFM_PSA_RW_STACK_START$$Base = .;

    .TFM_PSA_ROT_LINKER_DATA : ALIGN(32)
    {
        *tfm_psa_rot_partition*:*(.data*)
        *(TFM_*_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Base = ADDR(.TFM_PSA_ROT_LINKER_DATA);
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Limit = ADDR(.TFM_PSA_ROT_LINKER_DATA) + SIZEOF(.TFM_PSA_ROT_LINKER_DATA);

    .TFM_PSA_ROT_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_PSA_ROT_LINKER = .;
        *tfm_psa_rot_partition*:*(.bss*)
        *tfm_psa_rot_partition*:*(COMMON)
        *(TFM_*_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_PSA_ROT_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_PSA_ROT_LINKER_BSS);
    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_PSA_ROT_LINKER_BSS) + SIZEOF(.TFM_PSA_ROT_LINKER_BSS);

    /**** PSA RoT DATA end here */
    Image$$TFM_PSA_RW_STACK_END$$Base = .;

#ifdef RAM_VECTORS_SUPPORT
    .ramVectors (NOLOAD) : ALIGN(256)
    {
        __ram_vectors_start__ = .;
        KEEP(*(.ram_vectors))
        __ram_vectors_end__   = .;
    } > RAM
    .TFM_DATA __ram_vectors_end__ : ALIGN(4)
#else

     .TFM_DATA : ALIGN(4)
#endif
    {
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > RAM AT> FLASH
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS : ALIGN(4)
    {
        __bss_start__ = .;

        /* The runtime partition placed order is same as load partition */
        __partition_runtime_start__ = .;
        KEEP(*(.bss.part_runtime_priority_lowest))
        KEEP(*(.bss.part_runtime_priority_low))
        KEEP(*(.bss.part_runtime_priority_normal))
        KEEP(*(.bss.part_runtime_priority_high))
        __partition_runtime_end__ = .;
        . = ALIGN(4);

        /* The runtime service placed order is same as load partition */
        __service_runtime_start__ = .;
        KEEP(*(.bss.serv_runtime_priority_lowest))
        KEEP(*(.bss.serv_runtime_priority_low))
        KEEP(*(.bss.serv_runtime_priority_normal))
        KEEP(*(.bss.serv_runtime_priority_high))
        __service_runtime_end__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM AT> RAM
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);
    Image$$ER_PART_RT_POOL$$ZI$$Base = __partition_runtime_start__;
    Image$$ER_PART_RT_POOL$$ZI$$Limit = __partition_runtime_end__;
    Image$$ER_SERV_RT_POOL$$ZI$$Base = __service_runtime_start__;
    Image$$ER_SERV_RT_POOL$$ZI$$Limit = __service_runtime_end__;

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);

#if defined(PSA_PROXY_SHARED_MEMORY_BASE)
    /* If a variable defined with __attribute__((section())) keyword the
     * variable is treated like an initialized variable. To not waste memory
     * NOLOAD attribute used here. The whole section is zero initialized by
     * adding section information to .zero.table */
    .PSA_PROXY_SHARED_MEMORY (NOLOAD) : ALIGN(4)
    {
        KEEP(*(PSA_PROXY_SHARED_MEMORY_SECTION))
        .= ALIGN(4);
    } > PSA_PROXY_SHARED_MEMORY_RAM
#endif

#if defined (S_RAM_CODE_START)
    /* Code executed from RAM */
    .TFM_RAM_CODE S_RAM_CODE_START : ALIGN(4)
    {
        KEEP(*(.ramfunc))
        . = ALIGN(4);
    } > RAM AT> FLASH

    ASSERT(S_RAM_CODE_START % 4 == 0, "S_RAM_CODE_START must be divisible by 4")
#endif

#if VENEER_POS == 3
VENEERS()
#endif

    Load$$LR$$LR_NS_PARTITION$$Base = NS_PARTITION_START;

#ifdef NRF_NS_SECONDARY
    Load$$LR$$LR_SECONDARY_PARTITION$$Base = SECONDARY_PARTITION_START;
#endif /* NRF_NS_SECONDARY */

#ifdef NRF_NS_STORAGE_PARTITION_START
    Load$$LR$$LR_NRF_NS_STORAGE_PARTITION$$Base = NRF_NS_STORAGE_PARTITION_START;
#endif /* NRF_NS_STORAGE */

    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);
}
