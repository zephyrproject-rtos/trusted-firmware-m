/*
* Copyright (c) 2024, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#include "mscp_drv.h"

#include "tfm_hal_device_header.h"

#include <stdint.h>
#include <stddef.h>

struct _mscp_init_ctrl {
    __IOM uint32_t mscp_cpu_init_ctrl; /* 0x000 */
    const uint32_t reserved0[31];
    __IOM uint32_t tcmecc_errstatus; /* 0x080 */
    __IOM uint32_t tcmeec_errctrl; /* 0x084 */
    __IOM uint32_t tcmeec_errcode; /* 0x088 */
    __IM  uint32_t tcmeec_erraddr; /* 0x08C */
    const uint32_t reserved1[3];
    __IM  uint32_t mscp_icerr; /* 0x0A0 */
    __IM  uint32_t mscp_dcerr; /* 0x0A4 */
    const uint32_t reserved2[970];
    __IM  uint32_t pidr4; /* 0xFD0 */
    const uint32_t reserved3[3];
    __IM  uint32_t pidr0; /* 0xFE0 */
    __IM  uint32_t pidr1; /* 0xFE4 */
    __IM  uint32_t pidr2; /* 0xFE8 */
    __IM  uint32_t pidr3; /* 0xFEC */
    __IM  uint32_t cidr0; /* 0xFF0 */
    __IM  uint32_t cidr1; /* 0xFF4 */
    __IM  uint32_t cidr2; /* 0xFF8 */
    __IM  uint32_t cidr3; /* 0xFFC */
};

/* Field definitions for MSCP_CPU_INIT_CTRL */
#define MSCP_INIT_CPUCTRL_CPUWAIT_POS    (0)
#define MSCP_INIT_CPUCTRL_CPUWAIT_MASK   (0x1 << MSCP_INIT_CPUCTRL_CPUWAIT_POS)
#define MSCP_INIT_CPUCTRL_CPUWAIT_UNHALT (0x0 << MSCP_INIT_CPUCTRL_CPUWAIT_POS)
#define MSCP_INIT_CPUCTRL_CPUWAIT_HALT   (0x1 << MSCP_INIT_CPUCTRL_CPUWAIT_POS)

enum mscp_error_t mscp_driver_release_cpu(struct mscp_dev_t *dev)
{
    if (dev == NULL) {
        return MSCP_ERR_INVALID_PARAM;
    }

    struct _mscp_init_ctrl *mscp_init_ctrl =
            (struct _mscp_init_ctrl *) dev->init_ctrl_base;
    mscp_init_ctrl->mscp_cpu_init_ctrl &= ~MSCP_INIT_CPUCTRL_CPUWAIT_MASK;

    return MSCP_ERR_NONE;
}
