/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "tfm_hal_platform.h"

__WEAK void tfm_hal_system_reset(void)
{
    NVIC_SystemReset();
}
