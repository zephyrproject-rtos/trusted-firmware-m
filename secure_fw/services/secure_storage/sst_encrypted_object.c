/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_encrypted_object.h"

#include "crypto/sst_crypto_interface.h"
#include "sst_object_defs.h"
#include "sst_core.h"
#include "sst_utils.h"

/* Gets the size of data to encrypt */
#define SST_ENCRYPT_SIZE(plaintext_size) \
    (plaintext_size + SST_OBJECT_HEADER_SIZE - sizeof(union sst_crypto_t))

#define SST_CRYPTO_CLEAR_BUF_VALUE 0
#define SST_OBJECT_START_POSITION  0
#define SST_EMPTY_OBJECT_SIZE      0

/* Buffer to store the maximum encrypted object */
/* FIXME: Do partial encrypt/decrypt to reduce the size of internal buffer */
#define SST_MAX_ENCRYPTED_OBJ_SIZE  SST_ENCRYPT_SIZE(SST_MAX_OBJECT_DATA_SIZE)

static uint8_t sst_crypto_buf[SST_MAX_ENCRYPTED_OBJ_SIZE];

/**
 * \brief Gets the encryption key and sets it as the key to be used for
 *        cryptographic operations.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_set_encryption_key(void)
{
    enum psa_sst_err_t err;

    /* Key used for authenticated encryption and decryption */
    static uint8_t sst_encryption_key[SST_KEY_LEN_BYTES];

    /* Get the encryption key */
    err = sst_crypto_getkey(sst_encryption_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Set the key to be used for crypto operations */
    err = sst_crypto_setkey(sst_encryption_key, SST_KEY_LEN_BYTES);

    return err;
}

/**
 * \brief Performs authenticated decryption on object data, with the header as
 *        the associated data.
 *
 * \param[in]  fid       File ID
 * \param[in]  cur_size  Size of the object data to decrypt
 * \param[in/out] obj    Pointer to the object structure to authenticate and
 *                       fill in with the decrypted data. The tag of the object
 *                       is the one stored in the object table for the given
 *                       File ID.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_auth_decrypt(uint32_t fid,
                                                  uint32_t cur_size,
                                                  struct sst_object_t *obj)
{
    enum psa_sst_err_t err;
    uint8_t *p_obj_data = (uint8_t *)&obj->header.info;

    err = sst_object_set_encryption_key();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Use File ID as a part of the associated data to authenticate
     * the object in the FS. The tag will be stored in the object table and
     * not as a part of the object's data stored in the FS.
     */

    err = sst_crypto_auth_and_decrypt(&obj->header.crypto,
                                      (const uint8_t *)&fid,
                                      sizeof(fid),
                                      p_obj_data,
                                      sst_crypto_buf,
                                      cur_size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    sst_utils_memcpy(p_obj_data, sst_crypto_buf, cur_size);

    /* Clear the plaintext buffer from previous data */
    sst_utils_memset(sst_crypto_buf, SST_CRYPTO_CLEAR_BUF_VALUE,
                     sizeof(sst_crypto_buf));

    return PSA_SST_ERR_SUCCESS;
}

/**
 * \brief Performs authenticated encryption on object data, with the header as
 *        the associated data.
 *
 * \param[in]  fid       File ID
 * \param[in]  cur_size  Size of the object data to encrypt
 * \param[out] obj       Pointer to the object structure to authenticate and
 *                       fill in with the encrypted data.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_auth_encrypt(uint32_t fid,
                                                  uint32_t cur_size,
                                                  struct sst_object_t *obj)
{
    enum psa_sst_err_t err;
    uint8_t *p_obj_data = (uint8_t *)&obj->header.info;

    err = sst_object_set_encryption_key();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* FIXME: should have an IV per object with key diversification */
    /* Get a new IV for each encryption */
    sst_crypto_get_iv(&obj->header.crypto);

    /* Use File ID as a part of the associated data to authenticate
     * the object in the FS. The tag will be stored in the object table and
     * not as a part of the object's data stored in the FS.
     */

    err = sst_crypto_encrypt_and_tag(&obj->header.crypto,
                                     (const uint8_t *)&fid,
                                     sizeof(fid),
                                     p_obj_data,
                                     sst_crypto_buf,
                                     cur_size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    sst_utils_memcpy(p_obj_data, sst_crypto_buf, cur_size);

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_encrypted_object_read(uint32_t fid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct sst_object_t *obj)
{
    enum psa_sst_err_t err;
    struct sst_core_obj_info_t obj_info;
    uint32_t decrypt_size;

    /* FIXME: The token structure needs to be used when the key derivation
     *        mechanism is in place to generate the specific object key.
     */
    (void)s_token;

    /* Get the current size of the encrypted object */
    err = sst_core_object_get_info(fid, &obj_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Read the encrypted object from the the persistent area */
    err = sst_core_object_read(fid, obj->header.crypto.ref.iv,
                               SST_OBJECT_START_POSITION,
                               obj_info.size_current);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Get the decrypt size */
    decrypt_size = obj_info.size_current - sizeof(obj->header.crypto.ref.iv);

    /* Decrypt the object data */
    err = sst_object_auth_decrypt(fid, decrypt_size, obj);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_encrypted_object_write(uint32_t fid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct sst_object_t *obj)
{
    enum psa_sst_err_t err;
    uint32_t wrt_size;

    /* FIXME: The token structure needs to be used when the key derivation
     *        mechanism is in place to generate the specific object key.
     */
    (void)s_token;

    wrt_size = SST_ENCRYPT_SIZE(obj->header.info.size_max) +
               sizeof(obj->header.crypto.ref.iv);

    /* Create an object in the object system */
    err = sst_core_object_create(fid, wrt_size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    wrt_size = SST_ENCRYPT_SIZE(obj->header.info.size_current);

    /* Authenticate and encrypt the object */
    err = sst_object_auth_encrypt(fid, wrt_size, obj);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    wrt_size += sizeof(obj->header.crypto.ref.iv);

    /* Write the encrypted object to the persistent area. The tag values is not
     * copied as it is stored in the object table.
     */
    err = sst_core_object_write(fid, obj->header.crypto.ref.iv,
                                SST_OBJECT_START_POSITION, wrt_size);

    return err;
}
