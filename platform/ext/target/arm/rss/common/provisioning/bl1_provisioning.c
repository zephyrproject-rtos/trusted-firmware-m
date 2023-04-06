/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "log.h"
#include "rss_provisioning_bundle.h"
#include "tfm_plat_otp.h"
#include "device_definition.h"
#include "tfm_hal_platform.h"
#include <string.h>
#include "platform_regs.h"

enum rss_gpio_val_t {
    RSS_GPIO_STATE_VIRGIN_IDLE = 0x1,
    RSS_GPIO_STATE_CM_IDLE = 0x2,
    RSS_GPIO_STATE_RMA_IDLE = 0x3,
    RSS_GPIO_STATE_CM_SECURE_PROVISIONING_STARTS = 0x4,
    RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB = 0x5,
    RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR = 0x6,
    RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY = 0x7,
    RSS_GPIO_STATE_DM_IDLE = 0x8,
    RSS_GPIO_STATE_DM_SECURE_PROVISIONING_STARTS = 0x9,
    RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB = 0xa,
    RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_OTHER_ERROR = 0xb,
    RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY = 0xc,
    RSS_GPIO_STATE_SE_ROM_BOOT = 0xd,
};

static const volatile struct cm_provisioning_bundle *cm_encrypted_bundle =
(const struct cm_provisioning_bundle *)VM0_BASE_S;

static const volatile struct dm_provisioning_bundle *dm_encrypted_bundle =
(const struct dm_provisioning_bundle *)VM1_BASE_S;

static void gpio_set(enum rss_gpio_val_t val)
{
    volatile uint32_t *gretreg =
        &((struct rss_sysctrl_t *)RSS_SYSCTRL_BASE_S)->gretreg;

    *gretreg &= ~0b1111;
    *gretreg |= val & 0b1111;
}

void tfm_plat_provisioning_check_for_dummy_keys(void)
{
}

int tfm_plat_provisioning_is_required(void)
{
    enum lcm_error_t err;
    enum lcm_lcs_t lcs;
    bool provisioning_required;

    err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (err != LCM_ERROR_NONE) {
        return 1;
    }

    provisioning_required = (lcs == LCM_LCS_CM || lcs == LCM_LCS_DM);
    if (!provisioning_required) {
        if (lcs == LCM_LCS_RMA) {
            gpio_set(RSS_GPIO_STATE_RMA_IDLE);
        } else if (lcs == LCM_LCS_SE) {
            gpio_set(RSS_GPIO_STATE_SE_ROM_BOOT);
        }
    }

    return provisioning_required;
}

static enum tfm_plat_err_t enable_sp_mode(void)
{
    enum lcm_bool_t sp_enabled;
    enum lcm_error_t lcm_err;

    lcm_err = lcm_get_sp_enabled(&LCM_DEV_S, &sp_enabled);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (sp_enabled != LCM_TRUE) {
        BL1_LOG("[INF] Enabling secure provisioning mode, RSS will now reset.\r\n");
        lcm_set_sp_enabled(&LCM_DEV_S);
    }

    /* We'll never get here */
    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t provision_assembly_and_test(void)
{
    enum tfm_plat_err_t err;

    err = enable_sp_mode();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    gpio_set(RSS_GPIO_STATE_CM_SECURE_PROVISIONING_STARTS);

    /* TODO replace this with decrypt and auth */
    /* gpio_set(RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB); */
    memcpy((void*)PROVISIONING_BUNDLE_CODE_START,
           (void *)cm_encrypted_bundle->code,
           PROVISIONING_BUNDLE_CODE_SIZE);
    memcpy((void*)PROVISIONING_BUNDLE_VALUES_START,
           (void *)&cm_encrypted_bundle->values,
           PROVISIONING_BUNDLE_VALUES_SIZE + PROVISIONING_BUNDLE_DATA_SIZE);

    BL1_LOG("[INF] Running CM provisioning bundle\r\n");
    err = ((enum tfm_plat_err_t (*)(void))(PROVISIONING_BUNDLE_CODE_START | 0b1))();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
    }

    memset((void *)PROVISIONING_BUNDLE_CODE_START, 0,
           PROVISIONING_BUNDLE_CODE_SIZE);
    memset((void *)PROVISIONING_BUNDLE_VALUES_START, 0,
           PROVISIONING_BUNDLE_VALUES_SIZE + PROVISIONING_BUNDLE_DATA_SIZE);

    return err;
}

static enum tfm_plat_err_t provision_psa_rot(void)
{
    enum tfm_plat_err_t err;

    err = enable_sp_mode();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    gpio_set(RSS_GPIO_STATE_DM_SECURE_PROVISIONING_STARTS);

    /* TODO replace this with decrypt and auth */
    /* gpio_set(RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB); */
    memcpy((void *)PROVISIONING_BUNDLE_CODE_START,
           (void *)dm_encrypted_bundle->code,
           PROVISIONING_BUNDLE_CODE_SIZE);
    memcpy((void *)PROVISIONING_BUNDLE_VALUES_START,
           (void *)&dm_encrypted_bundle->values,
           PROVISIONING_BUNDLE_VALUES_SIZE + PROVISIONING_BUNDLE_DATA_SIZE);

    BL1_LOG("[INF] Running DM provisioning bundle\r\n");
    err = ((enum tfm_plat_err_t (*)(void))(PROVISIONING_BUNDLE_CODE_START | 0b1))();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
    }

    memset((void *)PROVISIONING_BUNDLE_CODE_START, 0,
           PROVISIONING_BUNDLE_CODE_SIZE);
    memset((void *)PROVISIONING_BUNDLE_VALUES_START, 0,
           PROVISIONING_BUNDLE_VALUES_SIZE + PROVISIONING_BUNDLE_DATA_SIZE);

    return err;
}

static enum tfm_plat_err_t set_tp_mode(void)
{
    volatile enum lcm_tp_mode_t tp_mode = LCM_TP_MODE_VIRGIN;
    enum lcm_error_t lcm_err;

#ifdef TFM_DUMMY_PROVISIONING
    tp_mode = LCM_TP_MODE_TCI;
#endif /* TFM_DUMMY_PROVISIONING */
    /* Loop until someone sets this with a debugger. DUMMY_PROVISIONING will
     * have set this to TCI.
     */
    while(tp_mode != LCM_TP_MODE_PCI && tp_mode != LCM_TP_MODE_TCI) {}

    lcm_err = lcm_set_tp_mode(&LCM_DEV_S, tp_mode);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    BL1_LOG("[INF] TP mode set complete, RSS will now reset.\r\n");
    tfm_hal_system_reset();
}

enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    enum tfm_plat_err_t err;
    enum lcm_error_t lcm_err;
    enum lcm_lcs_t lcs;
    enum lcm_tp_mode_t tp_mode;

    BL1_LOG("[INF] Beginning RSS provisioning\r\n");

    lcm_err = lcm_get_tp_mode(&LCM_DEV_S, &tp_mode);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    if (tp_mode == LCM_TP_MODE_VIRGIN) {
        gpio_set(RSS_GPIO_STATE_VIRGIN_IDLE);
        err = set_tp_mode();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    } else if (!(tp_mode == LCM_TP_MODE_TCI || tp_mode == LCM_TP_MODE_PCI)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (lcs == LCM_LCS_CM) {
        gpio_set(RSS_GPIO_STATE_CM_IDLE);

        BL1_LOG("[INF] Waiting for CM provisioning bundle\r\n");
        while (cm_encrypted_bundle->magic != CM_BUNDLE_MAGIC ||
               cm_encrypted_bundle->magic2 != CM_BUNDLE_MAGIC) {
        }

        err = provision_assembly_and_test();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        gpio_set(RSS_GPIO_STATE_CM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY);

#ifndef TFM_DUMMY_PROVISIONING
        __WFI();
#endif /* TFM_DUMMY_PROVISIONING */
    }

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    if (lcs == LCM_LCS_DM) {
        gpio_set(RSS_GPIO_STATE_DM_IDLE);

        BL1_LOG("[INF] Waiting for DM provisioning bundle\r\n");
        while (dm_encrypted_bundle->magic != DM_BUNDLE_MAGIC ||
               dm_encrypted_bundle->magic2 != DM_BUNDLE_MAGIC) {
        }

        err = provision_psa_rot();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        gpio_set(RSS_GPIO_STATE_DM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY);

#ifndef TFM_DUMMY_PROVISIONING
        __WFI();
#endif /* TFM_DUMMY_PROVISIONING */
    }

    return TFM_PLAT_ERR_SUCCESS;
}
