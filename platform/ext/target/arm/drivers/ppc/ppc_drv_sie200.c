/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ppc_drv.h"
#include <stdint.h>
#include <stdbool.h>

enum ppc_error_t
ppc_config_privilege(struct ppc_dev_t* dev, uint32_t mask,
                            enum ppc_sec_attr_t sec_attr,
                            enum ppc_priv_attr_t priv_attr)
{
    if(sec_attr == PPC_SECURE_ACCESS) {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
        /* Uses secure unprivileged access address (SACFG) to set privilege
         * attribute
         */
        if(priv_attr == PPC_PRIV_ONLY_ACCESS) {
            *(dev->cfg->p_sp_ppc) &= ~mask;
        } else {
            *(dev->cfg->p_sp_ppc) |= mask;
        }
#else
        /* Configuring security from Non-Secure application is not permitted. */
        return PPC_ERR_NOT_PERMITTED;
#endif
    } else {
        /* Uses non-secure unprivileged access address (NSACFG) to set
         * privilege attribute */
        if(priv_attr == PPC_PRIV_ONLY_ACCESS) {
            *(dev->cfg->p_nsp_ppc) &= ~mask;
        } else {
            *(dev->cfg->p_nsp_ppc) |= mask;
        }
    }

    return PPC_ERR_NONE;
}

bool ppc_is_periph_priv_only(struct ppc_dev_t* dev,
                                    uint32_t mask)
{
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    /* In secure domain either secure or non-secure privilege access is returned
     * based on the configuration */
    if ((*(dev->cfg->p_ns_ppc) & mask) == 0) {
        /* Returns secure unprivileged access (SACFG) */
        return ((*(dev->cfg->p_sp_ppc) & mask) == 0);
    } else {
        /* Returns non-secure unprivileged access (NSACFG) */
        return ((*(dev->cfg->p_nsp_ppc) & mask) == 0);
    }
#else
    /* Returns non-secure unprivileged access (NSACFG) */
    return ((*(dev->cfg->p_nsp_ppc) & mask) == 0);
#endif
}

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

enum ppc_error_t
ppc_config_security(struct ppc_dev_t* dev, uint32_t mask,
                           enum ppc_sec_attr_t sec_attr)
{
    if(sec_attr == PPC_SECURE_ACCESS) {
        *(dev->cfg->p_ns_ppc) &= ~mask;
    } else {
        *(dev->cfg->p_ns_ppc) |= mask;
    }

    return PPC_ERR_NONE;
}

bool ppc_is_periph_secure(struct ppc_dev_t* dev,
                                 uint32_t mask)
{
    return ((*(dev->cfg->p_ns_ppc) & mask) == 0);
}

void ppc_irq_enable(struct ppc_dev_t* dev)
{
    *(dev->int_regs->enable) |= dev->cfg->int_bit_mask;
}

void ppc_irq_disable(struct ppc_dev_t* dev)
{
    *(dev->int_regs->enable) &= ~(dev->cfg->int_bit_mask);
}

void ppc_clr_irq(struct ppc_dev_t* dev)
{
     *(dev->int_regs->clear) = dev->cfg->int_bit_mask;
}

bool ppc_irq_state(struct ppc_dev_t* dev)
{
    return ((*(dev->int_regs->status) & dev->cfg->int_bit_mask) != 0);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */
