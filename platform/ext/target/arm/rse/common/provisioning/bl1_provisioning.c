/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "log.h"
#include "rse_provisioning_bundle.h"
#include "tfm_plat_otp.h"
#include "device_definition.h"
#include "tfm_hal_platform.h"
#include <string.h>
#include "platform_regs.h"
#include "cc3xx_aes.h"
#include "rse_kmu_slot_ids.h"
#include "rse_kmu_keys.h"

enum rse_gpio_val_t {
    RSE_GPIO_STATE_VIRGIN_IDLE = 0x1,
    RSE_GPIO_STATE_CM_IDLE = 0x2,
    RSE_GPIO_STATE_RMA_IDLE = 0x3,
    RSE_GPIO_STATE_CM_SECURE_PROVISIONING_STARTS = 0x4,
    RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB = 0x5,
    RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR = 0x6,
    RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY = 0x7,
    RSE_GPIO_STATE_DM_IDLE = 0x8,
    RSE_GPIO_STATE_DM_SECURE_PROVISIONING_STARTS = 0x9,
    RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB = 0xa,
    RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_OTHER_ERROR = 0xb,
    RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY = 0xc,
    RSE_GPIO_STATE_SE_ROM_BOOT = 0xd,
};

static const volatile struct rse_provisioning_bundle *cm_encrypted_bundle =
(const struct rse_provisioning_bundle *)CM_PROVISIONING_BUNDLE_START;

static const volatile struct rse_provisioning_bundle *dm_encrypted_bundle =
(const struct rse_provisioning_bundle *)DM_PROVISIONING_BUNDLE_START;

static void gpio_set(enum rse_gpio_val_t val)
{
    volatile uint32_t *gretreg =
        &((struct rse_sysctrl_t *)RSE_SYSCTRL_BASE_S)->gretreg;

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
        return err;
    }

    provisioning_required = (lcs == LCM_LCS_CM || lcs == LCM_LCS_DM);
    if (!provisioning_required) {
        if (lcs == LCM_LCS_RMA) {
            gpio_set(RSE_GPIO_STATE_RMA_IDLE);
        } else if (lcs == LCM_LCS_SE) {
            gpio_set(RSE_GPIO_STATE_SE_ROM_BOOT);
        }
    }

    return provisioning_required;
}

#ifndef TEST_BL1_1
static
#endif
enum tfm_plat_err_t validate_and_unpack_encrypted_bundle(const volatile struct rse_provisioning_bundle *bundle,
                                              enum rse_kmu_slot_id_t key,
                                              uint8_t *code_output,
                                              size_t code_output_size,
                                              uint8_t *values_output,
                                              size_t values_output_size,
                                              uint8_t *data_output,
                                              size_t data_output_size)
{
    cc3xx_err_t cc_err;

    cc_err = cc3xx_lowlevel_aes_init(CC3XX_AES_DIRECTION_DECRYPT, CC3XX_AES_MODE_CCM,
                                     (cc3xx_aes_key_id_t)key, NULL,
                                     CC3XX_AES_KEYSIZE_256,
                                     (uint32_t *)bundle->iv,
                                     sizeof(bundle->iv));
    if (cc_err != CC3XX_ERR_SUCCESS) {
        BL1_LOG("[ERR] CC3XX setup failed\r\n");
        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
        return cc_err;
    }

    cc3xx_lowlevel_aes_set_tag_len(sizeof(bundle->tag));
    cc3xx_lowlevel_aes_set_data_len(offsetof(struct rse_provisioning_bundle, iv) -
                                    offsetof(struct rse_provisioning_bundle, code),
                                    sizeof(bundle->magic));

    cc3xx_lowlevel_aes_update_authed_data((uint8_t *)&bundle->magic,
                                          sizeof(bundle->magic));

    cc3xx_lowlevel_aes_set_output_buffer(code_output, code_output_size);
    cc_err = cc3xx_lowlevel_aes_update((uint8_t *)bundle->code, code_output_size);

    cc3xx_lowlevel_aes_set_output_buffer(values_output, values_output_size);
    cc3xx_lowlevel_aes_update((uint8_t *)bundle->values_as_bytes, values_output_size);

    cc3xx_lowlevel_aes_set_output_buffer(data_output, data_output_size);
    cc3xx_lowlevel_aes_update((uint8_t *)bundle->data, data_output_size);

    cc_err = cc3xx_lowlevel_aes_finish((uint32_t *)bundle->tag, NULL);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        BL1_LOG("[ERR] CM bundle decryption failed\r\n");
        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB);
        return cc_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

#ifndef TEST_BL1_1
static
#endif
enum tfm_plat_err_t setup_provisioning_key(bool is_cm)
{
    enum tfm_plat_err_t plat_err;

    if (is_cm) {
        plat_err = rse_setup_cm_provisioning_key();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    } else {
        plat_err = rse_setup_dm_provisioning_key();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

#ifndef TEST_BL1_1
static
#endif
enum tfm_plat_err_t provision_assembly_and_test(const volatile struct rse_provisioning_bundle *bundle,
                                                uint8_t *code_output,
                                                size_t code_output_size,
                                                uint8_t *values_output,
                                                size_t values_output_size,
                                                uint8_t *data_output,
                                                size_t data_output_size)
{
    enum tfm_plat_err_t plat_err;

    gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_STARTS);

    plat_err = setup_provisioning_key(true);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
        BL1_LOG("[ERR] CM provisioning key derivation failed\r\n");
        return plat_err;
    }

    plat_err = validate_and_unpack_encrypted_bundle(bundle,
                                         RSE_KMU_SLOT_CM_PROVISIONING_KEY,
                                         code_output, code_output_size,
                                         values_output, values_output_size,
                                         data_output, data_output_size);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        BL1_LOG("[ERR] CM bundle decryption failed\r\n");
        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB);
        return plat_err;
    }

    BL1_LOG("[INF] Running CM provisioning bundle\r\n");
    plat_err = ((enum tfm_plat_err_t (*)(void))((uintptr_t)code_output | 0b1))();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
    }

    memset(code_output, 0, code_output_size);
    memset(values_output, 0, values_output_size);
    memset(data_output, 0, data_output_size);

    return plat_err;
}

#ifndef TEST_BL1_1
static
#endif
enum tfm_plat_err_t provision_psa_rot(const volatile struct rse_provisioning_bundle *bundle,
                                      uint8_t *code_output,
                                      size_t code_output_size,
                                      uint8_t *values_output,
                                      size_t values_output_size,
                                      uint8_t *data_output,
                                      size_t data_output_size)
{
    enum tfm_plat_err_t plat_err;
    cc3xx_err_t cc_err;

    gpio_set(RSE_GPIO_STATE_DM_SECURE_PROVISIONING_STARTS);

    plat_err = setup_provisioning_key(false);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
        BL1_LOG("[ERR] DM provisioning key derivation failed\r\n");
        return plat_err;
    }

    plat_err = validate_and_unpack_encrypted_bundle(bundle,
                                         RSE_KMU_SLOT_DM_PROVISIONING_KEY,
                                         code_output, code_output_size,
                                         values_output, values_output_size,
                                         data_output, data_output_size);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        BL1_LOG("[ERR] DM bundle decryption failed\r\n");
        gpio_set(RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_NO_AUTHENTICATED_BLOB);
        return plat_err;
    }

    BL1_LOG("[INF] Running DM provisioning bundle\r\n");
    plat_err = ((enum tfm_plat_err_t (*)(void))((uintptr_t)code_output | 0b1))();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        gpio_set(RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FAILED_OTHER_ERROR);
    }

    memset(code_output, 0, code_output_size);
    memset(values_output, 0, values_output_size);
    memset(data_output, 0, data_output_size);

    return plat_err;
}

#ifndef TEST_BL1_1
static
#endif
enum tfm_plat_err_t set_tp_mode(void)
{
    volatile enum lcm_tp_mode_t tp_mode = LCM_TP_MODE_VIRGIN;
    enum lcm_error_t lcm_err;

#ifdef TFM_DUMMY_PROVISIONING
    tp_mode = RSE_TP_MODE;
#endif /* TFM_DUMMY_PROVISIONING */
    /* Loop until someone sets this with a debugger. DUMMY_PROVISIONING will
     * have set this to TCI.
     */
    while(tp_mode != LCM_TP_MODE_PCI && tp_mode != LCM_TP_MODE_TCI) {}

    lcm_err = lcm_set_tp_mode(&LCM_DEV_S, tp_mode);
    if (lcm_err != LCM_ERROR_NONE) {
        return lcm_err;
    }

    BL1_LOG("[INF] TP mode set complete, RSE will now reset.\r\n");
    tfm_hal_system_reset();
}

enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    enum tfm_plat_err_t plat_err;
    enum lcm_error_t lcm_err;
    enum lcm_lcs_t lcs;
    enum lcm_tp_mode_t tp_mode;

    BL1_LOG("[INF] Beginning RSE provisioning\r\n");

    lcm_err = lcm_get_tp_mode(&LCM_DEV_S, &tp_mode);
    if (lcm_err != LCM_ERROR_NONE) {
        return lcm_err;
    }
    if (tp_mode == LCM_TP_MODE_VIRGIN) {
        gpio_set(RSE_GPIO_STATE_VIRGIN_IDLE);
        plat_err = set_tp_mode();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    }

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return lcm_err;
    }

    if (lcs == LCM_LCS_CM) {
        gpio_set(RSE_GPIO_STATE_CM_IDLE);

        BL1_LOG("[INF] Waiting for CM provisioning bundle\r\n");
        while (cm_encrypted_bundle->magic != CM_BUNDLE_MAGIC ||
               cm_encrypted_bundle->magic2 != CM_BUNDLE_MAGIC) {
        }

        BL1_LOG("[INF] Enabling secure provisioning mode, RSE will now reset.\r\n");
        plat_err = tfm_plat_otp_secure_provisioning_start();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }

        plat_err = provision_assembly_and_test(cm_encrypted_bundle,
                                               (uint8_t *)PROVISIONING_BUNDLE_CODE_START,
                                               PROVISIONING_BUNDLE_CODE_SIZE,
                                               (uint8_t *)PROVISIONING_BUNDLE_VALUES_START,
                                               PROVISIONING_BUNDLE_VALUES_SIZE,
                                               (uint8_t *)PROVISIONING_BUNDLE_DATA_START,
                                               PROVISIONING_BUNDLE_DATA_SIZE);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            BL1_LOG("[ERR] CM provisioning failed\r\n");
            return plat_err;
        }

        gpio_set(RSE_GPIO_STATE_CM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY);
        BL1_LOG("[INF] CM provisioning succeeded\r\n");

        plat_err = tfm_plat_otp_secure_provisioning_finish();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    }

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return lcm_err;
    }

    if (lcs == LCM_LCS_DM) {
        gpio_set(RSE_GPIO_STATE_DM_IDLE);

        BL1_LOG("[INF] Waiting for DM provisioning bundle\r\n");
        while (dm_encrypted_bundle->magic != DM_BUNDLE_MAGIC ||
               dm_encrypted_bundle->magic2 != DM_BUNDLE_MAGIC) {
        }

        BL1_LOG("[INF] Enabling secure provisioning mode, RSE will now reset.\r\n");
        plat_err = tfm_plat_otp_secure_provisioning_start();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }

        plat_err = provision_psa_rot(dm_encrypted_bundle,
                                     (uint8_t *)PROVISIONING_BUNDLE_CODE_START,
                                     PROVISIONING_BUNDLE_CODE_SIZE,
                                     (uint8_t *)PROVISIONING_BUNDLE_VALUES_START,
                                     PROVISIONING_BUNDLE_VALUES_SIZE,
                                     (uint8_t *)PROVISIONING_BUNDLE_DATA_START,
                                     PROVISIONING_BUNDLE_DATA_SIZE);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            BL1_LOG("[ERR] DM provisioning failed\r\n");
            return plat_err;
        }

        gpio_set(RSE_GPIO_STATE_DM_SECURE_PROVISIONING_FINISHED_SUCCESSFULLY);
        BL1_LOG("[INF] DM provisioning succeeded\r\n");

        plat_err = tfm_plat_otp_secure_provisioning_finish();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
