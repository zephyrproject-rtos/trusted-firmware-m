/*
 * Copyright (c) 2022-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.9.0 startup_ARMCM55.c
 * Git SHA: 2b7495b8535bdcb306dac29b9ded4cfb679d7e5c
 */

#include "tfm_hal_device_header.h"

#include "device_definition.h"
#include "region_defs.h"
#include "rse_kmu_slot_ids.h"

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern void __PROGRAM_START(void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exception handler that blocks execution. */
__NO_RETURN void exception_handler(void)
{
    while (1);
}

/* No IRQs are enabled in BL1, so this handler should be unreachable. In case it
 * is ever reached, disable the IRQ that was triggered and return. In debug
 * builds, block execution to catch the bug.
 */
void invalid_irq_handler(void)
{
#ifndef NDEBUG
    while (1);
#else
    NVIC_DisableIRQ((IRQn_Type)((int32_t)__get_IPSR() - 16));
#endif
}

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[];
       const VECTOR_TABLE_Type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP), /*     Initial Stack Pointer */
  Reset_Handler,                     /*      Reset Handler */
  exception_handler,                 /* -14: NMI Handler */
  exception_handler,                 /* -13: Hard Fault Handler */
  exception_handler,                 /* -12: MPU Fault Handler */
  exception_handler,                 /* -11: Bus Fault Handler */
  exception_handler,                 /* -10: Usage Fault Handler */
  exception_handler,                 /*  -9: Secure Fault Handler */
  0,                                 /*      Reserved */
  0,                                 /*      Reserved */
  0,                                 /*      Reserved */
  exception_handler,                 /*  -5: SVCall Handler */
  exception_handler,                 /*  -4: Debug Monitor Handler */
  0,                                 /*      Reserved */
  exception_handler,                 /*  -2: PendSV Handler */
  exception_handler,                 /*  -1: SysTick Handler */

  invalid_irq_handler,               /*   0: Non-Secure Watchdog Reset Request Handler */
  invalid_irq_handler,               /*   1: Non-Secure Watchdog Handler */
  invalid_irq_handler,               /*   2: SLOWCLK Timer Handler */
  invalid_irq_handler,               /*   3: TIMER 0 Handler */
  invalid_irq_handler,               /*   4: TIMER 1 Handler */
  invalid_irq_handler,               /*   5: TIMER 2 Handler */
  0,                                 /*   6: Reserved */
  0,                                 /*   7: Reserved */
  0,                                 /*   8: Reserved */
  invalid_irq_handler,               /*   9: MPC Combined (Secure) Handler */
  invalid_irq_handler,               /*  10: PPC Combined (Secure) Handler */
  invalid_irq_handler,               /*  11: MSC Combined (Secure) Handler */
  invalid_irq_handler,               /*  12: Bridge Error (Secure) Handler */
  0,                                 /*  13: Reserved */
  invalid_irq_handler,               /*  14: PPU Combined (Secure) Handler */
  0,                                 /*  15: Reserved */
  invalid_irq_handler,               /*  16: NPU0 Handler */
  invalid_irq_handler,               /*  17: NPU1 Handler */
  invalid_irq_handler,               /*  18: NPU2 Handler */
  invalid_irq_handler,               /*  19: NPU3 Handler */
  invalid_irq_handler,               /*  20: KMU (Secure) Handler */
  0,                                 /*  21: Reserved */
  0,                                 /*  22: Reserved */
  0,                                 /*  23: Reserved */
  invalid_irq_handler,               /*  24: DMA350 Combined (Secure) Handler */
  invalid_irq_handler,               /*  25: DMA350 Combined (Non-Secure) Handler */
  invalid_irq_handler,               /*  26: DMA350 Security Violation Handler */
  invalid_irq_handler,               /*  27: TIMER 3 AON Handler */
  invalid_irq_handler,               /*  28: CPU0 CTI IRQ 0 Handler */
  invalid_irq_handler,               /*  29: CPU0 CTI IRQ 1 Handler */
  0,                                 /*  30: Reserved */
  0,                                 /*  31: Reserved */

  /* External interrupts */
  invalid_irq_handler,               /*  32: SAM Critical Security Fault (Secure) Handler */
  invalid_irq_handler,               /*  33: SAM Security Fault (Secure) Handler */
  invalid_irq_handler,               /*  34: GPIO Combined (Secure) Handler */
  invalid_irq_handler,               /*  35: Secure Debug Channel Handler */
  invalid_irq_handler,               /*  36: FPU Exception Handler */
  invalid_irq_handler,               /*  37: SRAM or TRAM Corrected ECC Error (Secure) Handler */
  invalid_irq_handler,               /*  38: Secure I-Cache (Secure) Handler */
  invalid_irq_handler,               /*  39: ATU (Secure) Handler */
  invalid_irq_handler,               /*  40: CMU MHU 0 Sender Handler */
  invalid_irq_handler,               /*  41: CMU MHU 0 Receiver Handler */
  invalid_irq_handler,               /*  42: CMU MHU 1 Sender Handler */
  invalid_irq_handler,               /*  43: CMU MHU 1 Receiver Handler */
  invalid_irq_handler,               /*  44: CMU MHU 2 Sender Handler */
  invalid_irq_handler,               /*  45: CMU MHU 2 Receiver Handler */
  invalid_irq_handler,               /*  46: CMU MHU 3 Sender Handler */
  invalid_irq_handler,               /*  47: CMU MHU 3 Receiver Handler */
  invalid_irq_handler,               /*  48: CMU MHU 4 Sender Handler */
  invalid_irq_handler,               /*  49: CMU MHU 4 Receiver Handler */
  invalid_irq_handler,               /*  50: CMU MHU 5 Sender Handler */
  invalid_irq_handler,               /*  51: CMU MHU 5 Receiver Handler */
  invalid_irq_handler,               /*  52: CMU MHU 6 Sender Handler */
  invalid_irq_handler,               /*  53: CMU MHU 6 Receiver Handler */
  invalid_irq_handler,               /*  54: CMU MHU 7 Sender Handler */
  invalid_irq_handler,               /*  55: CMU MHU 7 Receiver Handler */
  invalid_irq_handler,               /*  56: CMU MHU 8 Sender Handler */
  invalid_irq_handler,               /*  57: CMU MHU 8 Receiver Handler */
  invalid_irq_handler,               /*  58: Crypto Engine (Secure) Handler */
  invalid_irq_handler,               /*  59: SoC System Timer 0 AON Handler */
  invalid_irq_handler,               /*  60: SoC System Timer 1 AON Handler */
  invalid_irq_handler,               /*  61: SRAM ECC Detected Partial Write (Secure) Handler */
  invalid_irq_handler,               /*  62: Integrity Checker Handler */
  0,                                 /*  63: Reserved */
  0,                                 /*  64: Reserved */
  0,                                 /*  65: Reserved */
  0,                                 /*  66: Reserved */
  0,                                 /*  67: Reserved */
  0,                                 /*  68: Reserved */
  0,                                 /*  69: Reserved */
  0,                                 /*  70: Reserved */
  0,                                 /*  71: Reserved */
  0,                                 /*  72: Reserved */
  0,                                 /*  73: Reserved */
  0,                                 /*  74: Reserved */
  0,                                 /*  75: Reserved */
  0,                                 /*  76: Reserved */
  0,                                 /*  77: Reserved */
  0,                                 /*  78: Reserved */
  0,                                 /*  79: Reserved */
  0,                                 /*  80: Reserved */
  0,                                 /*  81: Reserved */
  0,                                 /*  82: Reserved */
  0,                                 /*  83: Reserved */
  0,                                 /*  84: Reserved */
  0,                                 /*  85: Reserved */
  0,                                 /*  86: Reserved */
  0,                                 /*  87: Reserved */
  0,                                 /*  88: Reserved */
  0,                                 /*  89: Reserved */
  0,                                 /*  90: Reserved */
  0,                                 /*  91: Reserved */
  0,                                 /*  92: Reserved */
  0,                                 /*  93: Reserved */
  0,                                 /*  94: Reserved */
  0,                                 /*  95: Reserved */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

#ifdef RSE_ENABLE_TRAM
static void setup_tram_encryption(void) {
    const struct kmu_key_export_config_t tram_key_export_config = {
        TRAM_BASE_S + 0x8, /* TRAM key register */
        0, /* No delay */
        0x01, /* Increment by 4 bytes with each write */
        KMU_DESTINATION_PORT_WIDTH_32_BITS, /* Write 32 bits with each write */
        KMU_DESTINATION_PORT_WIDTH_8_WRITES, /* Perform 8 writes (total 256 bits) */
        false, /* Don't refresh the masking */
        false, /* Don't disable the masking */
    };

    /* Redefine these, as at this point the constants haven't been loaded */
    const struct kmu_dev_cfg_t kmu_dev_cfg_s = {
        .base = KMU_BASE_S
    };
    const struct kmu_dev_t kmu_dev_s = {
        .cfg = &(kmu_dev_cfg_s)
    };
    const struct tram_dev_cfg_t tram_dev_cfg_s = {
    .base = TRAM_BASE_S
    };
    struct tram_dev_t tram_dev_s = {&tram_dev_cfg_s};


    /* Allow unaligned accesses */
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    __DSB();
    __ISB();

    /* Don't catch any errors from this, because we have no option to handle
     * them.
     */
    kmu_set_key_export_config(&kmu_dev_s, RSE_KMU_SLOT_TRAM_KEY, &tram_key_export_config);
    kmu_set_key_export_config_locked(&kmu_dev_s, RSE_KMU_SLOT_TRAM_KEY);
    kmu_set_key_locked(&kmu_dev_s, RSE_KMU_SLOT_TRAM_KEY);
    kmu_export_key(&kmu_dev_s, RSE_KMU_SLOT_TRAM_KEY);

    tram_enable_encryption(&tram_dev_s);
};
#endif /* RSE_ENABLE_TRAM */

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
#ifdef RSE_USE_ROM_LIB_FROM_SRAM
    /* At this point the GOT hasn't been set up, but we need it to set the stack
     * pointers. Copy the GOT into RAM first (before the usual data section
     * copy).
     */
    __asm volatile(
    "ldr    r1, =__etext \n"
    "ldr    r2, =__got_start__ \n"
    "ldr    r3, =__got_end__ \n"
    "subs    r3, r2 \n"
    "ble    .L_loop1_done \n"
    ".L_loop1: \n"
    "subs    r3, #4 \n"
    "ldr    r0, [r1,r3] \n"
    "str    r0, [r2,r3] \n"
    "bgt    .L_loop1 \n"
    ".L_loop1_done: \n"
    : : : "r1", "r2", "r3"
    );

    /* Load the base address of the GOT into r9. */
    __asm volatile(
        "mov r9, %0 \n"
        "mov r0, %1 \n"
        "lsl r9, #16 \n"
        "orr r9, r9, r0 \n"
        : : "I" (BL1_1_DATA_START >> 16), "I" (BL1_1_DATA_START & 0xFFFF) : "r0"
    );
#endif /* RSE_USE_ROM_LIB_FROM_SRAM */

#ifdef RSE_ENABLE_TRAM
    /* Set MSP to be in VM0 to start with */
    __set_MSP(CM_PROVISIONING_BUNDLE_START);
    __set_MSPLIM(VM0_BASE_S);

    setup_tram_encryption();

    /* Now switch back to the right stack (which is in the TRAM) */
    __set_MSPLIM(0);
    __set_MSP((uint32_t)(&__INITIAL_SP));
#endif /* RSE_ENABLE_TRAM */

    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));

    SystemInit();                    /* CMSIS System Initialization */
    __PROGRAM_START();               /* Enter PreMain (C library entry point) */
}
