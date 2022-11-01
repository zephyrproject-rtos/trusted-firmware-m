/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* The maximum asset size to be stored in the Protected Storage area. */
#if PS_MAX_ASSET_SIZE != 512
#pragma message ("PS_MAX_ASSET_SIZE is redefined to 512.")
#undef PS_MAX_ASSET_SIZE
#endif
#define PS_MAX_ASSET_SIZE    512

/* The maximum number of assets to be stored in the Protected Storage area. */
#if PS_NUM_ASSETS != 5
#pragma message ("PS_NUM_ASSETS is redefined to 5.")
#undef PS_NUM_ASSETS
#endif
#define PS_NUM_ASSETS        5

#endif /* __CONFIG_TFM_TARGET_H__ */
