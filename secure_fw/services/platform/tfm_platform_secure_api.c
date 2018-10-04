/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_platform_api.h"
#include "tfm_platform_veneers.h"

__attribute__((section("SFN")))
enum tfm_platform_err_t tfm_platform_system_reset(void)
{
    return tfm_platform_veneer_system_reset();
}
