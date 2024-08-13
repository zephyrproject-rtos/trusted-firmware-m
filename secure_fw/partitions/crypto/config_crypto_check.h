/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_PARTITION_CRYPTO_H__
#define __CONFIG_PARTITION_CRYPTO_H__

#include "config_tfm.h"

/* Check invalid configs. */
#if CRYPTO_NV_SEED && CRYPTO_EXT_RNG
#error "Invalid config: CRYPTO_NV_SEED AND CRYPTO_EXT_RNG!"
#endif

#if (!CRYPTO_NV_SEED) && (!CRYPTO_EXT_RNG)
#error "Invalid config: NOT CRYPTO_NV_SEED AND NOT CRYPTO_EXT_RNG!"
#endif

#endif /* __CONFIG_PARTITION_CRYPTO_H__ */
