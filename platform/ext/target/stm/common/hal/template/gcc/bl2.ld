;/*
; * Copyright (c) 2018-2022 ARM Limited
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
#if defined(BL2_NVMCNT_AREA_BASE)
  FLASH_NVMCNT(rx) : ORIGIN = BL2_NVMCNT_AREA_BASE, LENGTH = BL2_NVMCNT_AREA_SIZE
#endif
  FLASH (rx)  : ORIGIN = BL2_CODE_START, LENGTH = BL2_CODE_SIZE
  FLASH_NOHDP (rx)  : ORIGIN = BL2_NOHDP_CODE_START, LENGTH = BL2_NOHDP_CODE_SIZE
#if defined(BL2_OTP_AREA_BASE)
  FLASH_OTP(rx) : ORIGIN = BL2_OTP_AREA_BASE, LENGTH = BL2_OTP_AREA_SIZE
#endif
#if defined(BL2_NVM_AREA_BASE)
  FLASH_NVM(rx) : ORIGIN = BL2_NVM_AREA_BASE, LENGTH = BL2_NVM_AREA_SIZE
#endif
  RAM   (rwx) : ORIGIN = BL2_DATA_START, LENGTH = BL2_DATA_SIZE
}

__heap_size__  = BL2_HEAP_SIZE;
__msp_stack_size__ = BL2_MSP_STACK_SIZE;

ENTRY(Reset_Handler)

SECTIONS
{

#if defined(BL2_OTP_AREA_BASE)
    .BL2_OTP :
    {
       KEEP(*(.BL2_OTP_Const))
    } > FLASH_OTP
#endif

#if defined(BL2_NVM_AREA_BASE)
    .BL2_NVM :
    {
        KEEP(*(.BL2_NVM_Const))
    } > FLASH_NVM
#endif

#if defined(BL2_NVMCNT_AREA_BASE)
    .BL2_NVMCNT :
    {
        KEEP(*(.BL2_NVMCNT_Const))
    } > FLASH_NVMCNT
#endif

    .BL2_NoHdp_Code :
    {
        KEEP(*(.BL2_NoHdp_Data))
        KEEP(*(.BL2_NoHdp_Code))
        *mpu_armv8m_drv.o (.text* .rodata*)
        KEEP(*(.BL2_Error_Code))
        __hdp_end__ = .;
    } > FLASH_NOHDP

    .text (BL2_CODE_START) (READONLY) :
    {

        KEEP(*(.vectors))
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        *(.text*)

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

        /* .copy.table */
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__) / 4)
        LONG (DEFINED(__etext2) ? __etext2 : 0)
        LONG (DEFINED(__data2_start__) ? __data2_start__ : 0)
        LONG (DEFINED(__data2_start__) ? ((__data2_end__ - __data2_start__) / 4) : 0)
        __copy_table_end__ = .;

        /* .zero.table */
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG ((__bss_end__ - __bss_start__) / 4)
        LONG (DEFINED(__bss2_start__) ? __bss2_start__ : 0)
        LONG (DEFINED(__bss2_start__) ? ((__bss2_end__ - __bss2_start__) / 4) : 0)
        __zero_table_end__ = .;

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

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

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

    __etext = ALIGN(4);

    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > RAM

	.tdata : ALIGN_WITH_INPUT  {
		*(.tdata .tdata.* .gnu.linkonce.td.*)
		PROVIDE(__data_end = .);
		PROVIDE(__tdata_end = .);
        /* All data end */
        __data_end__ = .;
	} > RAM

    Image$$ER_CODE$$Base = __Vectors ;
    Image$$ER_CODE$$Limit = __etext + + SIZEOF(.data) + SIZEOF(.tdata);

	.tbss (NOLOAD) : {
        . = ALIGN(4);
        __bss_start__ = .;
		*(.tbss .tbss.* .gnu.linkonce.tb.*)
		*(.tcommon)
		PROVIDE( __tls_end = . );
		PROVIDE( __tbss_end = . );
	} > RAM

    .tbss_space (NOLOAD) : {
		. = ADDR(.tbss);
		. = . + SIZEOF(.tbss);
	} > RAM

    .bss :
    {
        . = ALIGN(4);
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
		PROVIDE(__bss_end = .);
    } > RAM

    bss_size = __bss_end__ - __bss_start__;

	PROVIDE( __tls_align = MAX(ALIGNOF(.tdata), ALIGNOF(.tbss)) );
	PROVIDE( __arm32_tls_tcb_offset = MAX(8, __tls_align) );
	PROVIDE(__data_source = LOADADDR(.data));
	PROVIDE(__data_start = ADDR(.data));
	PROVIDE( __data_size = __data_end - __data_start );
	PROVIDE( __bss_start = ADDR(.tbss));
	PROVIDE( __bss_size = __bss_end - __bss_start );
	PROVIDE( __tls_first = SIZEOF(.tdata) ? ADDR(.tdata) : ADDR(.tbss) );
	PROVIDE( __tls_base = __tls_first );

    .msp_stack :
    {
        . = ALIGN(8);
        KEEP(*(.psp_stack*))
        . += __msp_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.msp_stack) ;

    .heap :
    {
        . = ALIGN(8);
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM

    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);

    PROVIDE(__heap_start = ADDR(.heap));
    PROVIDE(__heap_end = ADDR(.heap) + SIZEOF(.heap));

}
