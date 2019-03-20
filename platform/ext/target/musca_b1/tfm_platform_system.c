/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "platform/include/tfm_platform_system.h"
#include "platform_description.h"
#include "target_cfg.h"
#include "device_definition.h"
#include "psa_client.h"
#include "tfm_platform_defs.h"
#include "tfm_secure_api.h"

/*!
 * \brief Verify access rights for memory addresses sent in io vectors
 *
 * \param[in] in_vec     Pointer to in_vec array, which contains pointer
 *                       to input arguments for the service
 * \param[in] out_vec    Pointer out_vec array, which contains pointer to
 *                       output data of the pin service
 *
 * \return Returns true if memory is accessible by the service
 */
static bool memory_addr_check(const psa_invec *in_vec,
                              const psa_outvec *out_vec)
{
    if ((in_vec->base != NULL) &&
        (tfm_core_memory_permission_check((void *)in_vec->base, in_vec->len,
                                        TFM_MEMORY_ACCESS_RO) == TFM_SUCCESS) &&
        (out_vec->base != NULL) &&
        (tfm_core_memory_permission_check((void *)out_vec->base, out_vec->len,
                                        TFM_MEMORY_ACCESS_RW) == TFM_SUCCESS)) {
        return true;
    } else {
        return false;
    }
}

void tfm_platform_hal_system_reset(void)
{
    __disable_irq();
    mpc_revert_non_secure_to_secure_cfg();

    NVIC->ICPR[0] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[1] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[2] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[3] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[4] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[5] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[6] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[7] = UINT32_MAX;         /* Clear all pending interrupts */

    NVIC_SystemReset();
}

enum tfm_plat_err_t
tfm_platform_hal_pin_service(const psa_invec  *in_vec,  uint32_t num_invec,
                             const psa_outvec *out_vec, uint32_t num_outvec)
{
    struct tfm_pin_service_args_t *args;
    uint32_t *result;
    enum gpio_altfunc_t altfunc;
    enum pinmode_select_t pin_mode;

    while (num_invec && num_outvec) {
        if (memory_addr_check(in_vec, out_vec) == false) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        if (in_vec->len != sizeof(struct tfm_pin_service_args_t) ||
                                             out_vec->len != sizeof(uint32_t)) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        args = (struct tfm_pin_service_args_t *)in_vec->base;
        result = (uint32_t *)out_vec->base;
        switch (args->type) {
        case TFM_PIN_SERVICE_TYPE_SET_ALTFUNC:
            altfunc = (enum gpio_altfunc_t)args->u.set_altfunc.alt_func;
            *result = musca_b1_scc_set_alt_func(&MUSCA_B1_SCC_DEV_S, altfunc,
                                                args->u.set_altfunc.pin_mask);
            break;
        case TFM_PIN_SERVICE_TYPE_SET_DEFAULT_IN:
            altfunc = (enum gpio_altfunc_t)args->u.set_altfunc.alt_func;
            *result = musca_b1_scc_set_default_in(&MUSCA_B1_SCC_DEV_S, altfunc,
                                       args->u.set_default_in.pin_value,
                                       args->u.set_default_in.default_in_value);
            break;
        case TFM_PIN_SERVICE_TYPE_SET_PIN_MODE:
            pin_mode = (enum pinmode_select_t)args->u.set_pin_mode.pin_mode;
            *result = musca_b1_scc_set_pinmode(&MUSCA_B1_SCC_DEV_S,
                                               args->u.set_pin_mode.pin_mask,
                                               pin_mode);
            break;
         default:
            *result = SCC_INVALID_ARG;
            break;
        }

        num_invec--;
        num_outvec--;
        in_vec++;
        out_vec++;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

