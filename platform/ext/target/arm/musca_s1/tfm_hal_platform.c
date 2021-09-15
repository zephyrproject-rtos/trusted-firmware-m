/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cache_drv.h"
#include "cmsis.h"
#include "cmsis_driver_config.h"
#include "musca_s1_scc_drv.h"
#include "tfm_hal_platform.h"
#include "uart_stdout.h"

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    musca_s1_scc_mram_fast_read_enable(&MUSCA_S1_SCC_DEV);

    arm_cache_enable_blocking(&SSE_200_CACHE_DEV);

    __enable_irq();
    stdio_init();

    return TFM_HAL_SUCCESS;
}
