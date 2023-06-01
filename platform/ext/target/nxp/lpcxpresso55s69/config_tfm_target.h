/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 * Copyright 2022 NXP. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Using of stored NV seed to provide entropy is disabled, when CRYPTO_HW_ACCELERATOR is defined.  */
#ifdef CRYPTO_HW_ACCELERATOR
#undef CRYPTO_NV_SEED
#define CRYPTO_NV_SEED       0
#endif

/* The maximum asset size to be stored in the Protected Storage area. */
#undef PS_MAX_ASSET_SIZE
#define PS_MAX_ASSET_SIZE    512

/* The maximum number of assets to be stored in the Protected Storage area. */
#undef PS_NUM_ASSETS
#define PS_NUM_ASSETS        5

/* The maximum number of assets to be stored in the Internal Trusted Storage */
#undef ITS_NUM_ASSETS
#define ITS_NUM_ASSETS       5


#ifdef PLATFORM_NO_FLASH
/* Enable emulated RAM FS for platforms that don't have flash for Internal Trusted Storage partition */
#undef ITS_RAM_FS
#define ITS_RAM_FS           1

/* Enable emulated RAM FS for platforms that don't have flash for Protected Storage partition */
#undef PS_RAM_FS
#define PS_RAM_FS            1
#endif /* PLATFORM_NO_FLASH */

#endif /* __CONFIG_TFM_TARGET_H__ */
