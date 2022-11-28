/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#undef CRYPTO_NV_SEED
#define CRYPTO_NV_SEED                         0

#ifdef TEST_PSA_API_CRYPTO
#undef CRYPTO_STACK_SIZE
#define CRYPTO_STACK_SIZE                      0x2100
#endif

#endif /* __CONFIG_TFM_TARGET_H__ */
