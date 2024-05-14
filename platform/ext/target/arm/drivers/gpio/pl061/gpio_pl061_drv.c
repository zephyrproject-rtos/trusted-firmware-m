/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file    gpio_pl061_drv.c
 * \brief   Driver for ARM Primecell GPIO Element
 *          As described in DDI0190
 *          https://developer.arm.com/documentation/ddi0190/latest/
 */

#include "gpio_pl061_drv.h"

uint32_t pl061_get_perifid(pl061_regblk_t * pdev)
{
    return ((pdev->gpioperiphid[0] & UINT8_MAX) << 0u |
            (pdev->gpioperiphid[1] & UINT8_MAX) << 8u |
            (pdev->gpioperiphid[2] & UINT8_MAX) << 16u |
            (pdev->gpioperiphid[3] & UINT8_MAX) << 24u);
}

uint32_t pl061_get_cellid(pl061_regblk_t * pdev)
{
    return ((pdev->gpiocellid[0] & UINT8_MAX) << 0u |
            (pdev->gpiocellid[1] & UINT8_MAX) << 8u |
            (pdev->gpiocellid[2] & UINT8_MAX) << 16u |
            (pdev->gpiocellid[3] & UINT8_MAX) << 24u);
}
