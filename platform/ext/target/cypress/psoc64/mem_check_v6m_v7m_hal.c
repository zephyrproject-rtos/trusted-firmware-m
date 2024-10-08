/*
 * Copyright (c) 2018-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "cy_prot.h"
#include "driver_smpu.h"
#include "mem_check_v6m_v7m_hal.h"
#include "pc_config.h"

/* Merge the second into the first, keeping the more restrictive */
static void combine_mem_attr(struct mem_attr_info_t *p_attr,
                             const struct mem_attr_info_t *p_attr2)
{
    /* If only one is valid, we need that one */
    if (p_attr->is_valid && !p_attr2->is_valid) {
        return;
    }
    if (!p_attr->is_valid && p_attr2->is_valid) {
        memcpy(p_attr, p_attr2, sizeof(*p_attr));
        return;
    }
    if (!p_attr->is_mpu_enabled) { p_attr->is_mpu_enabled = p_attr2->is_mpu_enabled; }
    if (!p_attr->is_xn) { p_attr->is_xn = p_attr2->is_xn; }
    if (p_attr->is_priv_rd_allow) { p_attr->is_priv_rd_allow = p_attr2->is_priv_rd_allow; }
    if (p_attr->is_priv_wr_allow) { p_attr->is_priv_wr_allow = p_attr2->is_priv_wr_allow; }
    if (p_attr->is_unpriv_rd_allow) { p_attr->is_unpriv_rd_allow = p_attr2->is_unpriv_rd_allow; }
    if (p_attr->is_unpriv_wr_allow) { p_attr->is_unpriv_wr_allow = p_attr2->is_unpriv_wr_allow; }
}

void tfm_hal_get_mem_security_attr(const void *p, size_t s,
                                   struct security_attr_info_t *p_attr)
{
    /* Rely on SPM */
    tfm_get_mem_region_security_attr(p, s, p_attr);
}

void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    uint32_t pc;
    struct mem_attr_info_t smpu_attr;

    SMPU_Get_Access_Rules(p, s, CY_PROT_SPM_DEFAULT, &smpu_attr);

    tfm_get_secure_mem_region_attr(p, s, p_attr);

    pc = Cy_Prot_GetActivePC(CPUSS_MS_ID_CM0);
    /* Check whether the current active PC is configured as the expected one .*/
    if (pc == CY_PROT_SPM_DEFAULT) {
        p_attr->is_mpu_enabled = true;
    } else {
        p_attr->is_mpu_enabled = false;
    }

    combine_mem_attr(p_attr, &smpu_attr);
}

void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr)
{
    struct mem_attr_info_t smpu_attr;

    SMPU_Get_Access_Rules(p, s, CY_PROT_HOST_DEFAULT, &smpu_attr);

    tfm_get_ns_mem_region_attr(p, s, p_attr);

    combine_mem_attr(p_attr, &smpu_attr);
}
