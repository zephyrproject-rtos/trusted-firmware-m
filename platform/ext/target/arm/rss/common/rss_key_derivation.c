/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_key_derivation.h"

#include "device_definition.h"
#include "crypto.h"
#include "otp.h"
#include "tfm_plat_otp.h"
#include "dpa_hardened_word_copy.h"
#include "cc3xx_drv.h"

#include <stdint.h>
#include <string.h>

extern uint8_t computed_bl1_2_hash[];

static int rss_get_boot_state(uint8_t *state, size_t state_buf_len,
                              size_t *state_size)
{
    int rc;
    enum plat_otp_lcs_t lcs;
    uint32_t reprovisioning_bits;

    if (state_buf_len < 32) {
        return 1;
    }

    rc = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t *)&lcs);
    if (rc) {
        return rc;
    }

    rc = tfm_plat_otp_read(PLAT_OTP_ID_REPROVISIONING_BITS,
                           sizeof(reprovisioning_bits),
                           (uint8_t *)&reprovisioning_bits);
    if (rc) {
        return rc;
    }

    rc = bl1_sha256_init();
    if (rc) {
        return rc;
    }

    rc = bl1_sha256_update((uint8_t *)&lcs, sizeof(lcs));
    if (rc) {
        return rc;
    }
    rc = bl1_sha256_update((uint8_t *)&reprovisioning_bits, sizeof(reprovisioning_bits));
    if (rc) {
        return rc;
    }
    rc = bl1_sha256_update(computed_bl1_2_hash, 32);
    if (rc) {
        return rc;
    }

    rc = bl1_sha256_finish(state);
    *state_size = 32;

    return 0;
}

static int rss_derive_key(enum tfm_bl1_key_id_t key_id, const uint8_t *label,
                          size_t label_len, enum rss_kmu_slot_id_t slot,
                          bool duplicate_into_next_slot)
{
    int rc;
    uint8_t context[32] = {0};
    size_t context_len;
    enum kmu_error_t kmu_err;
    volatile uint32_t *p_kmu_slot_buf;
    volatile uint32_t *p_kmu_secondary_slot_buf;
    size_t kmu_slot_size;

    rc = rss_get_boot_state(context, sizeof(context), &context_len);
    if (rc) {
        return rc;
    }

    kmu_err = kmu_get_key_buffer_ptr(&KMU_DEV_S, slot,
                                     &p_kmu_slot_buf, &kmu_slot_size);
    if (kmu_err != KMU_ERROR_NONE) {
        return -1;
    }

    rc = bl1_derive_key(key_id, label, label_len, context,
                        context_len, (uint8_t *)p_kmu_slot_buf, 32);
    if (rc) {
        return rc;
    }

    /* Due to limitations in CryptoCell, any key that needs to be used for
     * AES-CCM needs to be duplicated into a second slot.
     */
    if (duplicate_into_next_slot) {
        kmu_err = kmu_get_key_buffer_ptr(&KMU_DEV_S, slot + 1,
                                         &p_kmu_secondary_slot_buf,
                                         &kmu_slot_size);
        if (kmu_err != KMU_ERROR_NONE) {
            return -3;
        }

        dpa_hardened_word_copy(p_kmu_secondary_slot_buf, p_kmu_slot_buf,
                        kmu_slot_size / sizeof(uint32_t));

        /* TODO lock keyslots once the runtime CC3XX driver supports locked KMU
         * keyslots
         */
        /* kmu_err = kmu_set_key_locked(&KMU_DEV_S, slot + 1); */
        /* if (kmu_err != KMU_ERROR_NONE) { */
        /*     return -5; */
        /* } */
    }

    /* TODO lock keyslots once the runtime CC3XX driver supports locked KMU
     * keyslots
     */
    /* kmu_err = kmu_set_key_locked(&KMU_DEV_S, slot); */
    /* if (kmu_err != KMU_ERROR_NONE) { */
    /*     return -4; */
    /* } */

    return rc;
}

int rss_derive_cpak_seed(enum rss_kmu_slot_id_t slot)
{
    uint8_t cpak_seed_label[] = "BL1_CPAK_SEED_DERIVATION";

    return rss_derive_key(TFM_BL1_KEY_GUK, cpak_seed_label,
                          sizeof(cpak_seed_label), slot, false);
}

int rss_derive_dak_seed(enum rss_kmu_slot_id_t slot)
{
    uint8_t dak_seed_label[]  = "BL1_DAK_SEED_DERIVATION";

    return rss_derive_key(TFM_BL1_KEY_GUK, dak_seed_label,
                          sizeof(dak_seed_label), slot, false);
}

int rss_derive_vhuk_seed(uint32_t *vhuk_seed, size_t vhuk_seed_buf_len,
                         size_t *vhuk_seed_size)
{
    uint8_t vhuk_seed_label[]  = "VHUK_SEED_DERIVATION";
    int rc;

    if (vhuk_seed_buf_len != 32) {
        return 1;
    }

    rc = cc3xx_kdf_cmac(KMU_HW_SLOT_HUK, NULL, CC3XX_AES_KEYSIZE_256,
                        vhuk_seed_label, sizeof(vhuk_seed_label), NULL, 0,
                        vhuk_seed, 32);
    if (rc) {
        return rc;
    }

    *vhuk_seed_size = 32;

    return 0;
}

int rss_derive_vhuk(const uint8_t *vhuk_seeds, size_t vhuk_seeds_len,
                    enum rss_kmu_slot_id_t slot)
{
    return rss_derive_key(TFM_BL1_KEY_GUK, vhuk_seeds, vhuk_seeds_len,
                          slot, false);
}

int rss_derive_session_key(const uint8_t *ivs, size_t ivs_len,
                           enum rss_kmu_slot_id_t slot)
{
    return rss_derive_key(TFM_BL1_KEY_GUK, ivs, ivs_len, slot, true);
}
