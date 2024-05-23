/*
* Copyright (c) 2023, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#ifndef __RSE_EXPANSION_REGS_H__
#define __RSE_EXPANSION_REGS_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

__PACKED_STRUCT rse_integ_t {
    __IOM uint32_t rsecoreclk_ctrl; /* 0x000 */
    __IOM uint32_t rsecoreclk_div;  /* 0x004 */
    const uint32_t reserved0[2];
    __IOM uint32_t rse_integration; /* 0x010*/
    __IOM uint32_t atu_ap; /* 0x014 */
    const uint32_t reserved1[1005];
    __IM  uint32_t pidr4; /* 0xFD0 */
    const uint32_t reserved2[3];
    __IM  uint32_t pidr0; /* 0xFE0 */
    __IM  uint32_t pidr1; /* 0xFE4 */
    __IM  uint32_t pidr2; /* 0xFE8 */
    __IM  uint32_t pidr3; /* 0xFEC */
    __IM  uint32_t cidr0; /* 0xFF0 */
    __IM  uint32_t cidr1; /* 0xFF4 */
    __IM  uint32_t cidr2; /* 0xFF8 */
    __IM  uint32_t cidr3; /* 0xFFC */
};

/* Field definitions for RSECORECLK_CTRL register */
#define RSE_INTEG_CLKCTRL_SEL_POS      (0U)
#define RSE_INTEG_CLKCTRL_SEL_MSK      (0xFFUL << RSE_INTEG_CLKCTRL_SEL_POS)
#define RSE_INTEG_CLKCTRL_SEL_REF      (0x1UL << RSE_INTEG_CLKCTRL_SEL_POS)
#define RSE_INTEG_CLKCTRL_SEL_SPLL     (0x2UL << RSE_INTEG_CLKCTRL_SEL_POS)
#define RSE_INTEG_CLKCTRL_SEL_CUR_POS  (8U)
#define RSE_INTEG_CLKCTRL_SEL_CUR_MSK  (0xFFUL << RSE_INTEG_CLKCTRL_SEL_CUR_POS)
#define RSE_INTEG_CLKCTRL_SEL_CUR_REF  (0x1UL << RSE_INTEG_CLKCTRL_SEL_CUR_POS)
#define RSE_INTEG_CLKCTRL_SEL_CUR_SPLL (0x2UL << RSE_INTEG_CLKCTRL_SEL_CUR_POS)

/* Field definitions for RSECORECLK_DIV register */
#define RSE_INTEG_CLKDIV_DIV_POS       (0U)
#define RSE_INTEG_CLKDIV_DIV_MSK       (0xFUL << RSE_INTEG_CLKDIV_DIV_POS)
#define RSE_INTEG_CLKDIV_DIV_CUR_POS   (16U)
#define RSE_INTEG_CLKDIV_DIV_CUR_MSK   (0x4UL << RSE_INTEG_CLKDIV_DIV_CUR_POS)

/* Field definitions for RSE_INTEGRATION register */
#define RSE_INTEG_INTEG_SCP_RESET_POS  (0U)
#define RSE_INTEG_INTEG_SCP_RESET_MSK  (0x1UL << RSE_INTEG_INTEG_SCP_RESET_POS)
#define RSE_INTEG_INTEG_SCP_RESET      RSE_INTEG_INTEG_SCP_RESET_MSK
#define RSE_INTEG_INTEG_MCP_RESET_POS  (1U)
#define RSE_INTEG_INTEG_MCP_RESET_MSK  (0x1UL << RSE_INTEG_INTEG_MCP_RESET_POS)
#define RSE_INTEG_INTEG_MCP_RESET      RSE_INTEG_INTEG_MCP_RESET_MSK
#define RSE_INTEG_INTEG_SCP_RE_CLR_POS (3U)
#define RSE_INTEG_INTEG_SCP_RE_CLR_MSK (0x1UL << RSE_INTEG_INTEG_SCP_RE_CLR_POS)
#define RSE_INTEG_INTEG_SCP_RE_CLR     RSE_INTEG_INTEG_SCP_RE_CLR_MSK
#define RSE_INTEG_INTEG_MCP_RE_CLR_POS (4U)
#define RSE_INTEG_INTEG_MCP_RE_CLR_MSK (0x1UL << RSE_INTEG_INTEG_MCP_RE_CLR_POS)
#define RSE_INTEG_INTEG_CHIP_ID_POS    (8U)
#define RSE_INTEG_INTEG_CHIP_ID_MSK    (0xFUL << RSE_INTEG_INTEG_CHIP_ID_POS)
#define RSE_INTEG_INTEG_MCHIP_POS      (16U)
#define RSE_INTEG_INTEG_MCHIP_MSK      (0x1UL << RSE_INTEG_INTEG_MCHIP_POS)
#define RSE_INTEG_INTEG_MCHIP          RSE_INTEG_INTEG_MCHIP_MSK
#define RSE_INTEG_INTEG_SCP_SLEEP_POS  (17U)
#define RSE_INTEG_INTEG_SCP_SLEEP_MSK  (0x1UL << RSE_INTEG_INTEG_SCP_SLEEP_POS)
#define RSE_INTEG_INTEG_SCP_SLEEP      RSE_INTEG_INTEG_SCP_SLEEP_MSK
#define RSE_INTEG_INTEG_MCP_SLEEP_POS  (18U)
#define RSE_INTEG_INTEG_MCP_SLEEP_MSK  (0x1UL << RSE_INTEG_INTEG_MCP_SLEEP_POS)
#define RSE_INTEG_INTEG_MCP_SLEEP      RSE_INTEG_INTEG_MCP_SLEEP_MSK

/* Field definitions for ATU_AP register */
#define RSE_INTEG_ATU_AP_SCP_ATU_POS   (0U)
#define RSE_INTEG_ATU_AP_SCP_ATU_MSK   (0x1UL << RSE_INTEG_ATU_AP_SCP_ATU_POS)
#define RSE_INTEG_ATU_AP_SCP_ATU       RSE_INTEG_ATU_AP_SCP_ATU_MSK
#define RSE_INTEG_ATU_AP_MCP_ATU_POS   (1U)
#define RSE_INTEG_ATU_AP_MCP_ATU_MSK   (0x1UL << RSE_INTEG_ATU_AP_MCP_ATU_POS)
#define RSE_INTEG_ATU_AP_MCP_ATU       RSE_INTEG_ATU_AP_MCP_ATU_MSK

#endif /* __RSE_EXPANSION_REGS_H__ */
