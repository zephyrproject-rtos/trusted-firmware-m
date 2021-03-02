/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_platform.h"
#include "cmsis.h"
#include "uart_stdout.h"
#include "log/tfm_log.h"

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    __enable_irq();
    stdio_init();

    LOG_MSG("\033[1;34m[Platform] nRF5340 PDK is marked for deprecation!"
            "\033[0m\r\n");

    return TFM_HAL_SUCCESS;
}
