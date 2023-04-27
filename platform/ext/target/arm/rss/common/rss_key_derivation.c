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

#include <stdint.h>
#include <string.h>

extern uint8_t computed_bl1_2_hash[];

static int rss_derive_key(enum tfm_bl1_key_id_t key_id, const uint8_t *label,
                          size_t label_len, uint8_t *out)
{
    int rc;
    uint8_t context[32] = {0};
    size_t context_len;

    rc = rss_get_boot_state(context, sizeof(context), &context_len);
    if (rc) {
        return rc;
    }

    rc = bl1_derive_key(key_id, label, label_len, context,
                        context_len, out, 32);

    return rc;
}

int rss_get_boot_state(uint8_t *state, size_t state_buf_len,
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

static int set_and_lock_kmu_slot(uint8_t *key, uint32_t kmu_output_slot)
{
    enum kmu_error_t kmu_err;

    kmu_err = kmu_set_key(&KMU_DEV_S, kmu_output_slot, key, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return -1;
    }

    /* TODO lock the key slots once they can be used by the runtime CC driver */
    /* kmu_err = kmu_set_key_locked(&KMU_DEV_S, kmu_output_slot); */
    /* if (kmu_err != KMU_ERROR_NONE) { */
    /*     return -2; */
    /* } */

    return 0;
}

int rss_derive_vhuk_seed(uint8_t *vhuk_seed, size_t vhuk_seed_buf_len,
                         size_t *vhuk_seed_size)
{
    uint8_t vhuk_label[] = "BL1_VHUK_DERIVATION";
    int rc = 0;

    if (vhuk_seed_buf_len < 32) {
        return -1;
    }

    rc = rss_derive_key(TFM_BL1_KEY_HUK, vhuk_label,
                        sizeof(vhuk_label), vhuk_seed);
    if (rc) {
        return rc;
    }

    *vhuk_seed_size = 32;

    return 0;
}

int rss_derive_cpak_seed(uint32_t kmu_output_slot)
{
    uint8_t cpak_seed_label[] = "BL1_CPAK_SEED_DERIVATION";
    uint8_t __attribute__((__aligned__(4))) key_buf[32];
    int rc = 0;

    rc = rss_derive_key(TFM_BL1_KEY_GUK, cpak_seed_label,
                        sizeof(cpak_seed_label), key_buf);
    if (rc) {
        goto out;
    }

    rc = set_and_lock_kmu_slot(key_buf, kmu_output_slot);
    if (rc) {
        goto out;
    }

out:
    memset(key_buf, 0, sizeof(key_buf));

    return rc;
}

int rss_derive_dak_seed(uint32_t kmu_output_slot)
{
    uint8_t dak_seed_label[]  = "BL1_DAK_SEED_DERIVATION";
    uint8_t __attribute__((__aligned__(4))) key_buf[32];
    int rc = 0;

    rc = rss_derive_key(TFM_BL1_KEY_GUK, dak_seed_label,
                        sizeof(dak_seed_label), key_buf);
    if (rc) {
        goto out;
    }

    rc = set_and_lock_kmu_slot(key_buf, kmu_output_slot);
    if (rc) {
        goto out;
    }

out:
    memset(key_buf, 0, sizeof(key_buf));

    return rc;
}

int rss_derive_vhuk(const uint8_t *vhuk_seeds, size_t vhuk_seeds_len,
                    uint32_t kmu_output_slot)
{
    uint8_t __attribute__((__aligned__(4))) key_buf[32];
    int rc;

    if (vhuk_seeds_len != RSS_AMOUNT * 32) {
        return -1;
    }

    rc = bl1_sha256_compute(vhuk_seeds, vhuk_seeds_len, key_buf);
    if (rc) {
        return rc;
    }

    rc = set_and_lock_kmu_slot(key_buf, kmu_output_slot);
    if (rc) {
        goto out;
    }

out:
    memset(key_buf, 0, sizeof(key_buf));

    return rc;
}

int rss_derive_session_key(const uint8_t *ivs, size_t ivs_len,
                           uint32_t kmu_output_slot)
{
    uint8_t common_iv[32];
    uint8_t __attribute__((__aligned__(4))) key_buf[32];
    int rc;

    if (ivs_len != RSS_AMOUNT * 32) {
        return -1;
    }

    rc = bl1_sha256_compute(ivs, ivs_len, common_iv);
    if (rc) {
        return rc;
    }

    rc = rss_derive_key(TFM_BL1_KEY_GUK, common_iv,
                        sizeof(common_iv), key_buf);
    if (rc) {
        goto out;
    }

    rc = set_and_lock_kmu_slot(key_buf, kmu_output_slot);
    if (rc) {
        goto out;
    }

out:
    memset(key_buf, 0, sizeof(key_buf));

    return rc;
}

