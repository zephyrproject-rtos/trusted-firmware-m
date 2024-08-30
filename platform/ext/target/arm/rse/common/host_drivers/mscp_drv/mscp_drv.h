/*
* Copyright (c) 2024, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#ifndef __MSCP_DRV_H__
#define __MSCP_DRV_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum mscp_error_t {
    /* No Error */
    MSCP_ERR_NONE,
    /* Invalid parameter */
    MSCP_ERR_INVALID_PARAM,
    /* Error accessing mscp */
    MSCP_ERR_ACCESS,
    /* General error with driver */
    MSCP_ERR_GENERAL,
};

struct mscp_dev_t {
    /* Base address of the init control register bank */
    const uintptr_t init_ctrl_base;
};

enum mscp_error_t mscp_driver_release_cpu(struct mscp_dev_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* __MSCP_DRV_H__ */
