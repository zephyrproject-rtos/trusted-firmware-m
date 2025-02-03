/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "region_defs.h"
#include "rse_provisioning_message_handler.h"
#include "rse_provisioning_comms.h"
#include "device_definition.h"
#include "tfm_log.h"
#include "tfm_hal_platform.h"
#include "rse_otp_dev.h"
#include "tfm_plat_otp.h"
#include "rse_kmu_slot_ids.h"
#include "rse_kmu_keys.h"
#include "rse_provisioning_rotpk.h"
#include "fatal_error.h"

#include <string.h>

#define PROVISIONING_MESSAGE_START (VM0_BASE_S + OTP_DMA_ICS_SIZE)
#define RSE_PROVISIONING_MESSAGE_MAX_SIZE (VM1_BASE_S - PROVISIONING_MESSAGE_START)

static const struct rse_provisioning_message_t *provisioning_message =
(const struct rse_provisioning_message_t *)PROVISIONING_MESSAGE_START;

static inline bool blob_is_combined(const struct rse_provisioning_message_blob_t *blob)
{
    return (((blob->purpose >> RSE_PROVISIONING_BLOB_PURPOSE_TYPE_OFFSET)
             & RSE_PROVISIONING_BLOB_PURPOSE_TYPE_MASK)
            == RSE_PROVISIONING_BLOB_TYPE_COMBINED_LCS_PROVISIONING);
}

static enum tfm_plat_err_t aes_setup_key(const struct rse_provisioning_message_blob_t *blob,
                                         uint32_t *key_id)
{
    enum lcm_error_t lcm_err;
    enum tfm_plat_err_t err;
    enum lcm_lcs_t lcs;
    uint8_t *label;
    uint8_t label_len = 0;
    uint8_t *context;
    size_t context_len = 0;

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    if (lcs != LCM_LCS_CM && lcs != LCM_LCS_DM) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_DERIVATION_INVALID_LCS);
        return TFM_PLAT_ERR_PROVISIONING_DERIVATION_INVALID_LCS;
    }

    if (lcs == LCM_LCS_CM || blob_is_combined(blob)) {
        label = (uint8_t *)"KMASTER_CM";
        label_len = sizeof("KMASTER_CM");
        context = (uint8_t *)&blob->batch_id;
        context_len = sizeof(blob->batch_id);
    } else {
        label = (uint8_t *)"KMASTER_DM";
        label_len = sizeof("KMASTER_DM");
        context = (uint8_t *)&blob->dm_number;
        context_len = sizeof(blob->dm_number);
    }

    err = rse_setup_master_key(label, label_len, context, context_len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    *key_id = RSE_KMU_SLOT_PROVISIONING_KEY;

    if (lcs == LCM_LCS_CM || blob_is_combined(blob)) {
        label = (uint8_t *)"KPROV_CM";
        label_len = sizeof("KPROV_CM");
    } else {
        label = (uint8_t *)"KPROV_DM";
        label_len = sizeof("KPROV_DM");
    }

    return rse_setup_provisioning_key(label, label_len, NULL, 0);
}

static enum tfm_plat_err_t rotpk_get(const struct rse_provisioning_message_blob_t *blob,
                                     uint32_t **public_key_x,
                                     size_t *public_key_x_size,
                                     uint32_t **public_key_y,
                                     size_t *public_key_y_size)
{
    return provisioning_rotpk_get(public_key_x,
                                  public_key_x_size,
                                  public_key_y,
                                  public_key_y_size);
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

    return provisioning_required;
}

enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    INFO("Beginning RSE provisioning\n");
    enum tfm_plat_err_t err;
    size_t msg_len;

    struct provisioning_message_handler_config config = {
        .blob_handler = &default_blob_handler,
    };

    struct default_blob_handler_ctx_t ctx = {
        .setup_aes_key = aes_setup_key,
#ifdef RSE_PROVISIONING_ENABLE_ECDSA_SIGNATURES
        .get_rotpk = rotpk_get,
#endif
        .blob_is_chainloaded = false,
    };

    if (provisioning_message->header.data_length == 0) {
        err = provisioning_comms_receive(provisioning_message,
                                         RSE_PROVISIONING_MESSAGE_MAX_SIZE,
                                         &msg_len);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    /* FixMe: Check if the current way of handling blobs can result in running
     *        a malformed blob in case a reset happens in the middle of an
     *        operation which has already written the blob header
     */
    err = handle_provisioning_message(provisioning_message,
                                      RSE_PROVISIONING_MESSAGE_MAX_SIZE,
                                      &config, (void *)&ctx);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        provisioning_comms_return_status(err);
        memset((void *)provisioning_message, 0, RSE_PROVISIONING_MESSAGE_MAX_SIZE);
        return err;
    }

    tfm_hal_system_reset();

    return TFM_PLAT_ERR_SUCCESS;
}
