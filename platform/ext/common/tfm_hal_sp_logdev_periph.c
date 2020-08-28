/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_sp_logdev.h"
#include "uart_stdout.h"

int32_t tfm_hal_output_sp_log(const unsigned char *str, size_t len)
{
    /* Peripheral based log function call the stdio_output_string directly */
    return stdio_output_string(str, len);
}
