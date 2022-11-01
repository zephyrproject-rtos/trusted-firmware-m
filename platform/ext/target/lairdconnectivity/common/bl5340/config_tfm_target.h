/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Size of output buffer in platform service. */
#if ITS_NUM_ASSETS != 5
#pragma message ("ITS_NUM_ASSETS is redefined to 5.")
#undef ITS_NUM_ASSETS
#endif
#define ITS_NUM_ASSETS    5

#endif /* __CONFIG_TFM_TARGET_H__ */
