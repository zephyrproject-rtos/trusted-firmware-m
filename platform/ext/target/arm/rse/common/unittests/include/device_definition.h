/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __DEVICE_DEFINITION_H__
#define __DEVICE_DEFINITION_H__

#ifdef KMU_S
#include "kmu_drv.h"
extern struct kmu_dev_t KMU_DEV_S;
#endif

#ifdef LCM_S
#include "lcm_drv.h"
extern struct lcm_dev_t LCM_DEV_S;
#endif

#ifdef GPIO0_S
#include "gpio_pl061_drv.h"
extern pl061_regblk_t *const GPIO0_DEV_S;
#endif

#endif  /* __DEVICE_DEFINITION_H__ */
