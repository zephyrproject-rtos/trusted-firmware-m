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
#include "host_base_address.h"
#include "platform_base_address.h"
#include "uart_stdout.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);

static int32_t init_atu_regions(void)
{
    enum atu_error_t err;

    /* Initialize UART region */
    err = atu_initialize_region(&ATU_DEV_S,
                                get_supported_region_count(&ATU_DEV_S) - 1,
                                UART0_BASE_NS, HOST_UART_BASE, HOST_UART_SIZE);
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
