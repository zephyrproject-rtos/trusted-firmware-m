/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "tfm_crypto_defs.h"
#include "tfm_secure_api.h"

enum tfm_crypto_err_t tfm_crypto_memory_check(void *addr,
                                              uint32_t size,                                
                                              uint32_t access)
{
    return tfm_core_memory_permission_check(addr, size, access);
}
