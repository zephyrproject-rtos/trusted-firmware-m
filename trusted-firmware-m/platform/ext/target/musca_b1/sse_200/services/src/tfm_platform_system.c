/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_platform_system.h"
#include "platform_description.h"
#include "target_cfg.h"
#include "device_definition.h"
#include "psa/client.h"
#include "tfm_ioctl_api.h"

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

static enum tfm_platform_err_t
musca_b1_pin_service(const psa_invec  *in_vec,
                     const psa_outvec *out_vec)
{
    struct tfm_pin_service_args_t *args;
    uint32_t *result;
    enum gpio_altfunc_t altfunc;
    enum pinmode_select_t pin_mode;

    if (in_vec->len != sizeof(struct tfm_pin_service_args_t) ||
                                         out_vec->len != sizeof(uint32_t)) {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
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

    return TFM_PLATFORM_ERR_SUCCESS;
}

enum tfm_platform_err_t
tfm_platform_hal_gpio_service(const psa_invec  *in_vec,
                              const psa_outvec *out_vec)
{
    struct tfm_gpio_service_args_t *args;
    struct tfm_gpio_service_out_t *out;
    enum gpio_cmsdk_direction_t dir;
    /* Alternate function is configured through the SCC, this is not used
     * on Musca-B1, the default value is passed to the driver
     */
    enum gpio_cmsdk_altfunc_t altfunc = GPIO_CMSDK_MAIN_FUNC;

    if (in_vec->len != sizeof(struct tfm_gpio_service_args_t) ||
        out_vec->len != sizeof(struct tfm_gpio_service_out_t)) {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
    }

    args = (struct tfm_gpio_service_args_t *)in_vec->base;
    out = (struct tfm_gpio_service_out_t *)out_vec->base;
    switch (args->type) {
    case TFM_GPIO_SERVICE_TYPE_INIT:
        gpio_cmsdk_init(&GPIO0_CMSDK_DEV_S);
        out->u.result = GPIO_CMSDK_ERR_NONE;
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_CONFIG:
        dir = (enum gpio_cmsdk_direction_t)args->u.gpio_config.direction;
        out->u.result = gpio_cmsdk_pin_config(
                                            &GPIO0_CMSDK_DEV_S,
                                            args->u.gpio_config.pin_num_or_mask,
                                            dir, altfunc);
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_WRITE:
        out->u.result = gpio_cmsdk_pin_write(
                                             &GPIO0_CMSDK_DEV_S,
                                             args->u.gpio_write.pin_num_or_mask,
                                             args->u.gpio_write.value);
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_READ:
        out->u.gpio_read_result.result =
                          gpio_cmsdk_pin_read(&GPIO0_CMSDK_DEV_S,
                                              args->u.gpio_read.pin_num_or_mask,
                                              &out->u.gpio_read_result.data);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_CONFIG:
        dir = (enum gpio_cmsdk_direction_t)args->u.gpio_config.direction;
        out->u.result = gpio_cmsdk_port_config(
                                            &GPIO0_CMSDK_DEV_S,
                                            args->u.gpio_config.pin_num_or_mask,
                                            dir, altfunc);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_WRITE:
        out->u.result = gpio_cmsdk_port_write(
                                             &GPIO0_CMSDK_DEV_S,
                                             args->u.gpio_write.pin_num_or_mask,
                                             args->u.gpio_write.value);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_READ:
        out->u.gpio_read_result.result =
                         gpio_cmsdk_port_read(&GPIO0_CMSDK_DEV_S,
                                              args->u.gpio_read.pin_num_or_mask,
                                              &out->u.gpio_read_result.data);
        break;
    default:
        out->u.result = GPIO_CMSDK_ERR_INVALID_ARG;
        break;
    }

    in_vec++;
    out_vec++;

    return TFM_PLATFORM_ERR_SUCCESS;
}

enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    switch (request) {
    case TFM_PLATFORM_IOCTL_PIN_SERVICE:
        return musca_b1_pin_service(in_vec, out_vec);
    case TFM_PLATFORM_IOCTL_GPIO_SERVICE:
        return tfm_platform_hal_gpio_service(in_vec, out_vec);
    default:
        return TFM_PLATFORM_ERR_NOT_SUPPORTED;
    }
}
