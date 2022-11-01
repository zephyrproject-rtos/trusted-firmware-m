/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#if CRYPTO_NV_SEED
#pragma message ("CRYPTO_NV_SEED is redefined to 0.")
#undef CRYPTO_NV_SEED
#endif
#define CRYPTO_NV_SEED                         0

/* Size of output buffer in platform service. */
#if PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE != 256
#pragma message ("PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE is redefined to 256.")
#undef PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE
#endif
#define PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE    256

#endif /* __CONFIG_TFM_TARGET_H__ */
