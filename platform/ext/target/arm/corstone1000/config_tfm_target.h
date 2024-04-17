/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#define CRYPTO_NV_SEED                         0

/* Size of output buffer in platform service. */
#define PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE    256

/* The maximum number of assets to be stored in the Internal Trusted Storage. */
#define ITS_NUM_ASSETS       20

/* The maximum number of assets to be stored in the Protected Storage area. */
#define PS_NUM_ASSETS        20

/* The maximum size of asset to be stored in the Internal Trusted Storage area. */
#define ITS_MAX_ASSET_SIZE   2048

#endif /* __CONFIG_TFM_TARGET_H__ */
