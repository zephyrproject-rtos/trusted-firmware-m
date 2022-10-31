/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_PARTITION_SPM_H__
#define __CONFIG_PARTITION_SPM_H__

#include "config_impl.h"
#include "config_tfm.h"

/* The maximal number of secure services that are connected or requested at the same time */
#ifndef CONFIG_TFM_CONN_HANDLE_MAX_NUM
#pragma message ("CONFIG_TFM_CONN_HANDLE_MAX_NUM is defaulted to 8. Please check and set it explicitly.")
#define CONFIG_TFM_CONN_HANDLE_MAX_NUM 8
#endif

/* Enable the doorbell APIs */
#ifndef CONFIG_TFM_DOORBELL_API
#pragma message ("CONFIG_TFM_DOORBELL_API is defaulted to 1. Please check and set it explicitly.")
#define CONFIG_TFM_DOORBELL_API        1
#endif

/* Disable doorbell for SFN backend */
#if CONFIG_TFM_SPM_BACKEND_SFN == 1
#pragma message ("CONFIG_TFM_DOORBELL_API is redefined to 0.")
#undef  CONFIG_TFM_DOORBELL_API
#define CONFIG_TFM_DOORBELL_API        0
#endif

#endif /* __CONFIG_PARTITION_SPM_H__ */
