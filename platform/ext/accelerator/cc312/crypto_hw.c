/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdint.h>
#include "cc3xx_dcu.h"

int crypto_hw_apply_debug_permissions(uint8_t *permissions_mask, uint32_t len)
{
    return (cc3xx_dcu_set_enabled(permissions_mask, len) == CC3XX_ERR_SUCCESS) ? 0 : -1;
}
