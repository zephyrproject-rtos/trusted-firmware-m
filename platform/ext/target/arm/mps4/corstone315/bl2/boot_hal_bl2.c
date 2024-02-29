/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_device_header.h"

int32_t boot_platform_post_init(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    return 0;
}
