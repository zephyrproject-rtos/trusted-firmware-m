/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_crypto_interface.h"

#include <stdbool.h>

#include "tfm_crypto_defs.h"
#include "psa/crypto.h"
#include "tfm_memory_utils.h"

/* The PSA key type used by this implementation */
#define SST_KEY_TYPE PSA_KEY_TYPE_AES
/* The PSA key usage required by this implementation */
#define SST_KEY_USAGE (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT)
/* The PSA algorithm used by this implementation */
#define SST_CRYPTO_ALG \
    PSA_ALG_AEAD_WITH_TAG_LENGTH(PSA_ALG_GCM, SST_TAG_LEN_BYTES)

static const uint8_t sst_key_label[] = "storage_key";
static psa_key_handle_t sst_key_handle;
static uint8_t sst_crypto_iv_buf[SST_IV_LEN_BYTES];

psa_ps_status_t sst_crypto_init(void)
{
    /* Currently, no initialisation is required. This may change if key
     * handling is changed.
     */
    return PSA_PS_SUCCESS;
}

psa_ps_status_t sst_crypto_setkey(void)
{
    psa_status_t status;
    psa_key_handle_t huk_key_handle;
    psa_key_policy_t key_policy = PSA_KEY_POLICY_INIT;
    psa_crypto_generator_t sst_key_generator = PSA_CRYPTO_GENERATOR_INIT;

    /* Allocate a transient key handle for the storage key */
    status = psa_allocate_key(&sst_key_handle);
    if (status != PSA_SUCCESS) {
        return PSA_PS_ERROR_OPERATION_FAILED;
    }

    /* Set the key policy for the storage key */
    psa_key_policy_set_usage(&key_policy, SST_KEY_USAGE, SST_CRYPTO_ALG);
    status = psa_set_key_policy(sst_key_handle, &key_policy);
    if (status != PSA_SUCCESS) {
        goto release_sst_key;
    }

    /* Open a handle to the HUK */
    status = psa_open_key(PSA_KEY_LIFETIME_PERSISTENT, TFM_CRYPTO_KEY_ID_HUK,
                          &huk_key_handle);
    if (status != PSA_SUCCESS) {
        goto release_sst_key;
    }

    /* Set up a key derivation operation with the HUK as the input key */
    status = psa_key_derivation(&sst_key_generator,
                                huk_key_handle,
                                TFM_CRYPTO_ALG_HUK_DERIVATION,
                                NULL, 0,
                                sst_key_label, sizeof(sst_key_label),
                                SST_KEY_LEN_BYTES);
    if (status != PSA_SUCCESS) {
        goto release_huk;
    }

    /* Create the storage key from the key generator */
    status = psa_generator_import_key(sst_key_handle,
                                      SST_KEY_TYPE,
                                      PSA_BYTES_TO_BITS(SST_KEY_LEN_BYTES),
                                      &sst_key_generator);
    if (status != PSA_SUCCESS) {
        goto release_generator;
    }

    /* Free resources allocated by the generator */
    status = psa_generator_abort(&sst_key_generator);
    if (status != PSA_SUCCESS) {
        goto release_huk;
    }

    /* Close the handle to the HUK */
    status = psa_close_key(huk_key_handle);
    if (status != PSA_SUCCESS) {
        goto release_sst_key;
    }

    return PSA_PS_SUCCESS;

release_generator:
    (void)psa_generator_abort(&sst_key_generator);

release_huk:
    (void)psa_close_key(huk_key_handle);

release_sst_key:
    (void)psa_destroy_key(sst_key_handle);

    return PSA_PS_ERROR_OPERATION_FAILED;
}

psa_ps_status_t sst_crypto_destroykey(void)
{
    psa_status_t status;

    /* Destroy the transient key */
    status = psa_destroy_key(sst_key_handle);
    if (status != PSA_SUCCESS) {
        return PSA_PS_ERROR_OPERATION_FAILED;
    }

    return PSA_PS_SUCCESS;
}

void sst_crypto_set_iv(const union sst_crypto_t *crypto)
{
    (void)tfm_memcpy(sst_crypto_iv_buf, crypto->ref.iv, SST_IV_LEN_BYTES);
}

void sst_crypto_get_iv(union sst_crypto_t *crypto)
{
    /* IV characteristic is algorithm dependent.
     * For GCM it is essential that it doesn't get repeated.
     * A simple increment will suffice.
     * FIXME:
     * Since IV is predictable in this case,
     * If there is no rollback protection, an attacker could
     * try to rollback the storage and encrypt another plaintext
     * block with same IV/Key pair; this breaks GCM usage rules.
     * One potential fix would be to generate IV through RNG
     */

    /* Logic:
     * IV is a 12 byte value. Read the old value and increment it by 1.
     * since there is no standard C support for 12 byte integer mathematics,
     * the increment need to performed manually. Increment the lower 8byte
     * as uint64_t value and then if the new value is 0, increment the upper
     * 4 bytes as uint32_t
     * Endian order doesn't really matter as objective is not to perform
     * machine accurate increment operation but to generate a non-repetitive
     * iv value.
     */

    uint64_t iv_l;
    uint32_t iv_h;

    (void)tfm_memcpy(&iv_l, sst_crypto_iv_buf, sizeof(iv_l));
    (void)tfm_memcpy(&iv_h, (sst_crypto_iv_buf+sizeof(iv_l)), sizeof(iv_h));
    iv_l++;
    /* If overflow, increment the MSBs */
    if (iv_l == 0) {
        iv_h++;
    }

    /* Update the local buffer */
    (void)tfm_memcpy(sst_crypto_iv_buf, &iv_l, sizeof(iv_l));
    (void)tfm_memcpy((sst_crypto_iv_buf + sizeof(iv_l)), &iv_h, sizeof(iv_h));
    /* Update the caller buffer */
    (void)tfm_memcpy(crypto->ref.iv, sst_crypto_iv_buf, SST_IV_LEN_BYTES);
}

psa_ps_status_t sst_crypto_encrypt_and_tag(union sst_crypto_t *crypto,
                                           const uint8_t *add,
                                           size_t add_len,
                                           const uint8_t *in,
                                           size_t in_len,
                                           uint8_t *out,
                                           size_t out_size,
                                           size_t *out_len)
{
    psa_status_t status;

    status = psa_aead_encrypt(sst_key_handle, SST_CRYPTO_ALG,
                              crypto->ref.iv, SST_IV_LEN_BYTES,
                              add, add_len,
                              in, in_len,
                              out, out_size, out_len);
    if (status != PSA_SUCCESS) {
        return PSA_PS_ERROR_OPERATION_FAILED;
    }

    /* Copy the tag out of the output buffer */
    *out_len -= SST_TAG_LEN_BYTES;
    (void)tfm_memcpy(crypto->ref.tag, (out + *out_len), SST_TAG_LEN_BYTES);

    return PSA_PS_SUCCESS;
}

psa_ps_status_t sst_crypto_auth_and_decrypt(const union sst_crypto_t *crypto,
                                            const uint8_t *add,
                                            size_t add_len,
                                            uint8_t *in,
                                            size_t in_len,
                                            uint8_t *out,
                                            size_t out_size,
                                            size_t *out_len)
{
    psa_status_t status;

    /* Copy the tag into the input buffer */
    (void)tfm_memcpy((in + in_len), crypto->ref.tag, SST_TAG_LEN_BYTES);
    in_len += SST_TAG_LEN_BYTES;

    status = psa_aead_decrypt(sst_key_handle, SST_CRYPTO_ALG,
                              crypto->ref.iv, SST_IV_LEN_BYTES,
                              add, add_len,
                              in, in_len,
                              out, out_size, out_len);
    if (status != PSA_SUCCESS) {
        return PSA_PS_ERROR_AUTH_FAILED;
    }

    return PSA_PS_SUCCESS;
}

psa_ps_status_t sst_crypto_generate_auth_tag(union sst_crypto_t *crypto,
                                             const uint8_t *add,
                                             uint32_t add_len)
{
    psa_status_t status;
    size_t out_len;

    status = psa_aead_encrypt(sst_key_handle, SST_CRYPTO_ALG,
                              crypto->ref.iv, SST_IV_LEN_BYTES,
                              add, add_len,
                              0, 0,
                              crypto->ref.tag, SST_TAG_LEN_BYTES, &out_len);
    if (status != PSA_SUCCESS || out_len != SST_TAG_LEN_BYTES) {
        return PSA_PS_ERROR_OPERATION_FAILED;
    }

    return PSA_PS_SUCCESS;
}

psa_ps_status_t sst_crypto_authenticate(const union sst_crypto_t *crypto,
                                        const uint8_t *add,
                                        uint32_t add_len)
{
    psa_status_t status;
    size_t out_len;

    status = psa_aead_decrypt(sst_key_handle, SST_CRYPTO_ALG,
                              crypto->ref.iv, SST_IV_LEN_BYTES,
                              add, add_len,
                              crypto->ref.tag, SST_TAG_LEN_BYTES,
                              0, 0, &out_len);
    if (status != PSA_SUCCESS || out_len != 0) {
        return PSA_PS_ERROR_AUTH_FAILED;
    }

    return PSA_PS_SUCCESS;
}
