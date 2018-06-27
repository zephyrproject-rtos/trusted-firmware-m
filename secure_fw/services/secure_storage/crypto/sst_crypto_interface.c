/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_crypto_interface.h"
#include "secure_fw/services/secure_storage/sst_utils.h"

/* pre include mbedTLS headers */
#define LIB_PREFIX_NAME __tfm_storage__
#include "mbedtls_global_symbols.h"

#define MBEDTLS_CONFIG_FILE "mbedtls_config.h"
#include "mbedtls/gcm.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "platform/include/plat_crypto_keys.h"

/* FIXME: most of this implementation will be replaced by crypto service API
 *        calls
 */

#define SST_MBEDTLS_MEM_BUF_LEN 512

static mbedtls_gcm_context sst_crypto_gcm_ctx = { { 0 } };
static uint8_t sst_crypto_iv_buf[SST_IV_LEN_BYTES];

/* Static buffer to be used by mbedtls for memory allocation */
static uint8_t mbedtls_mem_buf[SST_MBEDTLS_MEM_BUF_LEN];

enum psa_sst_err_t sst_crypto_init(void)
{
    mbedtls_gcm_free(&sst_crypto_gcm_ctx);

    /* Initialise the mbedtls static memory allocator so that mbedtls allocates
     * memory from the provided static buffer instead of from the heap.
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf, SST_MBEDTLS_MEM_BUF_LEN);

    mbedtls_gcm_init(&sst_crypto_gcm_ctx);

    /* Currently returns SUCCESS as the mbedtls functions called
     * are void. When integrated with crypto engine or service
     * a return value may be required.
     */
    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_crypto_getkey(uint8_t *key, size_t key_len)
{
    /* FIXME: if key diversification is desired, the appid
     * can be used to derive a key from the HUK derived key.
     * However, this is tricky for shared resources which can
     * be accessed by multiple clients (i.e. multiple appID)
     *
     * To be fixed in later revisions. Currently, just use the
     * same HUK (derived) key for all the crypto operations.
     */
    plat_get_crypto_huk(key, key_len);

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_crypto_setkey(const uint8_t *key, size_t key_len)
{
    return mbedtls_gcm_setkey(&sst_crypto_gcm_ctx, MBEDTLS_CIPHER_ID_AES,
                              key, key_len*8);
}

void sst_crypto_set_iv(const union sst_crypto_t *crypto)
{
    sst_utils_memcpy(sst_crypto_iv_buf, crypto->ref.iv, SST_IV_LEN_BYTES);
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

    sst_utils_memcpy(&iv_l, sst_crypto_iv_buf, sizeof(iv_l));
    sst_utils_memcpy(&iv_h, (sst_crypto_iv_buf+sizeof(iv_l)), sizeof(iv_h));
    iv_l++;
    /* If overflow, increment the MSBs */
    if (iv_l == 0) {
        iv_h++;
    }

    /* Update the local buffer */
    sst_utils_memcpy(sst_crypto_iv_buf, &iv_l, sizeof(iv_l));
    sst_utils_memcpy((sst_crypto_iv_buf + sizeof(iv_l)), &iv_h, sizeof(iv_h));
    /* Update the caller buffer */
    sst_utils_memcpy(crypto->ref.iv, sst_crypto_iv_buf, SST_IV_LEN_BYTES);

}

enum psa_sst_err_t sst_crypto_encrypt_and_tag(
                                            union sst_crypto_t *crypto,
                                            const uint8_t *add, size_t add_len,
                                            const uint8_t *in, uint8_t *out,
                                            size_t len)
{
    return mbedtls_gcm_crypt_and_tag(&sst_crypto_gcm_ctx, MBEDTLS_GCM_ENCRYPT,
                                     len, crypto->ref.iv, SST_IV_LEN_BYTES, add,
                                     add_len, in, out, SST_TAG_LEN_BYTES,
                                     crypto->ref.tag);
}

enum psa_sst_err_t sst_crypto_auth_and_decrypt(
                                             const union sst_crypto_t *crypto,
                                             const uint8_t *add, size_t add_len,
                                             const uint8_t *in, uint8_t *out,
                                             size_t len)
{
    return mbedtls_gcm_auth_decrypt(&sst_crypto_gcm_ctx, len, crypto->ref.iv,
                                    SST_IV_LEN_BYTES, add, add_len,
                                    crypto->ref.tag, SST_TAG_LEN_BYTES,
                                    in, out);
}

enum psa_sst_err_t sst_crypto_generate_auth_tag(union sst_crypto_t *crypto,
                                                const uint8_t *add,
                                                uint32_t add_len)
{
    enum psa_sst_err_t ret;

    ret = sst_crypto_encrypt_and_tag(crypto, add, add_len, 0, 0, 0);
    return ret;
}

enum psa_sst_err_t sst_crypto_authenticate(
                                         const union sst_crypto_t *crypto,
                                         const uint8_t *add, uint32_t add_len)
{
    uint32_t ret;

    ret = sst_crypto_auth_and_decrypt(crypto, add, add_len, 0, 0, 0);
    return ret;
}
