/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "tfm_hal_platform.h"
#include "uart_stdout.h"

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    __enable_irq();
    stdio_init();

    return TFM_HAL_SUCCESS;
}
