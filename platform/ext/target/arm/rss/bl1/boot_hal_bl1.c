/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"
#include "region.h"
#include "device_definition.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "platform_base_address.h"
#include "uart_stdout.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);

/* ATU config */
#define ATU_UART_LOG_ADDRESS       UART0_BASE_NS
#define ATU_UART_PHYS_ADDRESS      ((uint64_t)0x2A400000)
#define ATU_UART_REGION_SIZE       0x2000 /* 8KB */

#define ATU_SCP_SRAM_LOG_ADDRESS   SCP_SRAM_BASE_S
#define ATU_SCP_SRAM_PHYS_ADDRESS  ((uint64_t)0x40000000)
#define ATU_SCP_SRAM_REGION_SIZE   SCP_BL1_SIZE /* 64KB */

#define ATU_AP_SRAM_LOG_ADDRESS    AP_SRAM_BASE_S
#define ATU_AP_SRAM_PHYS_ADDRESS   ((uint64_t)0x0)
#define ATU_AP_SRAM_REGION_SIZE    AP_BL1_SIZE /* 128KB */

static int32_t init_atu_regions(void)
{
    enum atu_error_t err;

    /* Initialize UART region */
    err = atu_initialize_region(&ATU_DEV_S, 0, ATU_UART_LOG_ADDRESS,
                                ATU_UART_PHYS_ADDRESS, ATU_UART_REGION_SIZE);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    /* Initialize SCP region */
    err = atu_initialize_region(&ATU_DEV_S, 1, ATU_SCP_SRAM_LOG_ADDRESS,
                                ATU_SCP_SRAM_PHYS_ADDRESS,
                                ATU_SCP_SRAM_REGION_SIZE);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    /* Initialize AP region */
    err = atu_initialize_region(&ATU_DEV_S, 2, ATU_AP_SRAM_LOG_ADDRESS,
                                ATU_AP_SRAM_PHYS_ADDRESS,
                                ATU_AP_SRAM_REGION_SIZE);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    return 0;
}

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;

    /* Initialize stack limit register */
    uint32_t msp_stack_bottom =
            (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);

    __set_MSPLIM(msp_stack_bottom);

    result = init_atu_regions();
    if (result) {
        return result;
    }

#if defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2)
    stdio_init();
#endif /* defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2) */

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

    return 0;
}
