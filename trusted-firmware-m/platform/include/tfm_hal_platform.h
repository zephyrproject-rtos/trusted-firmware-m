/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_HAL_PLATFORM_H__
#define __TFM_HAL_PLATFORM_H__

#include "tfm_hal_defs.h"

/**
 * \brief This function performs the platform-specific initialization.
 *
 * This function is called after architecture and platform common initialization
 * has finished during system early startup.
 *
 * \retval TFM_HAL_SUCCESS          Init success.
 * \retval TFM_HAL_ERROR_GENERIC    Generic errors.
 */
enum tfm_hal_status_t tfm_hal_platform_init(void);

/**
 * \brief System reset
 */
void tfm_hal_system_reset(void);

#endif /* __TFM_HAL_PLATFORM_H__ */
