/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_attest_hal.h"

enum tfm_security_lifecycle_t tfm_attest_hal_get_security_lifecycle(void)
{
    return TFM_SLC_SECURED;
}
