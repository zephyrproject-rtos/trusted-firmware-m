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

/* Gets the size of an encrypted object written to the object system below */
#define SST_ENCRYPTED_SIZE(plaintext_size) \
    (plaintext_size + SST_OBJECT_HEADER_SIZE)

/* Gets the size of the decrypted object data returned to the client above */
#define SST_PLAINTEXT_SIZE(encrypted_size) \
    (encrypted_size - SST_OBJECT_HEADER_SIZE)

/* The associated data is the header, minus the crypto metadata */
#define SST_CRYPTO_ASSOCIATED_DATA ((uint8_t *)&obj->header.crypto + \
                                     sizeof(union sst_crypto_t))
/* The associated data is the header, minus the crypto metadata */
#define SST_CRYPTO_ASSOCIATED_DATA_LEN (SST_OBJECT_HEADER_SIZE - \
                                        sizeof(union sst_crypto_t))

#define SST_CRYPTO_CLEAR_BUF_VALUE 0
#define SST_OBJECT_START_POSITION  0
#define SST_EMPTY_OBJECT_SIZE      0

/* Buffer to store plaintext object data */
static uint8_t sst_plaintext_buf[SST_MAX_OBJECT_DATA_SIZE];

/**
 * \brief Gets the encryption key and sets it as the key to be used for
 *        cryptographic operations.
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
static enum tfm_sst_err_t sst_object_set_encryption_key(void)
{
    enum tfm_sst_err_t err;

    /* Key used for authenticated encryption and decryption */
    static uint8_t sst_encryption_key[SST_KEY_LEN_BYTES];

    /* Get the encryption key */
    err = sst_crypto_getkey(sst_encryption_key, SST_KEY_LEN_BYTES);
    if (err != TFM_SST_ERR_SUCCESS) {
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
 * \param[in]  cur_size  Size of the object data
 * \param[out] obj       Pointer to the object structure to fill in
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
static enum tfm_sst_err_t sst_object_auth_decrypt(uint32_t cur_size,
                                                  struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;

    err = sst_object_set_encryption_key();
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Clear the plaintext buffer of any previous data */
    sst_utils_memset(sst_plaintext_buf, SST_CRYPTO_CLEAR_BUF_VALUE,
                     sizeof(sst_plaintext_buf));
    err = sst_crypto_auth_and_decrypt(&obj->header.crypto,
                                      SST_CRYPTO_ASSOCIATED_DATA,
                                      SST_CRYPTO_ASSOCIATED_DATA_LEN,
                                      obj->data,
                                      sst_plaintext_buf,
                                      cur_size);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    sst_utils_memcpy(obj->data, sst_plaintext_buf,
                     obj->header.info.size_current);

    return TFM_SST_ERR_SUCCESS;
}

/**
 * \brief Performs authenticated encryption on object data, with the header as
 *        the associated data.
 *
 * \param[in]  cur_size  Size of the object data
 * \param[out] obj       Pointer to the object structure to fill in
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
static enum tfm_sst_err_t sst_object_auth_encrypt(uint32_t cur_size,
                                                  struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;

    err = sst_object_set_encryption_key();
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* FIXME: should have an IV per object with key diversification */
    /* Get a new IV for each encryption */
    sst_crypto_get_iv(&obj->header.crypto);

    err = sst_crypto_encrypt_and_tag(&obj->header.crypto,
                                     SST_CRYPTO_ASSOCIATED_DATA,
                                     SST_CRYPTO_ASSOCIATED_DATA_LEN,
                                     obj->data,
                                     sst_plaintext_buf,
                                     cur_size);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    sst_utils_memcpy(obj->data, sst_plaintext_buf, cur_size);

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_encrypted_object_create(uint32_t uuid,
                                               struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;

    /* Create an object in the object system */
    err = sst_core_object_create(uuid,
                                 SST_ENCRYPTED_SIZE(obj->header.info.size_max));
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Authenticate the header, with no data to encrypt */
    err = sst_object_auth_encrypt(SST_EMPTY_OBJECT_SIZE, obj);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Write the encrypted object to the object system */
    err = sst_core_object_write(uuid, (uint8_t *)obj,
                                SST_OBJECT_START_POSITION,
                                SST_ENCRYPTED_SIZE(SST_EMPTY_OBJECT_SIZE));

    return err;
}

/**
 * \brief Reads and decrypts data from the object referenced by the object
 *        UUID into the sst_plaintext_buf buffer.
 *
 * \param[in]  uuid  Object UUID
 * \param[out] obj   Pointer to the object structure to fill in
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
static enum tfm_sst_err_t sst_read_encrypted_object(uint32_t uuid,
                                                    struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;
    struct sst_core_obj_info_t obj_info;
    uint32_t plaintext_size;

    /* Get the current size of the encrypted object in the object system */
    err = sst_core_object_get_info(uuid, &obj_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Get the current plaintext size of the object data */
    plaintext_size = SST_PLAINTEXT_SIZE(obj_info.size_current);

    /* Clear the object buffer of any previous data */
    sst_utils_memset(obj, SST_CRYPTO_CLEAR_BUF_VALUE, SST_MAX_OBJECT_SIZE);

    /* Read the encrypted object from the object system */
    err = sst_core_object_read(uuid, (uint8_t *)obj,
                               SST_OBJECT_START_POSITION,
                               obj_info.size_current);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Decrypt the object data */
    err = sst_object_auth_decrypt(plaintext_size, obj);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_encrypted_object_read(uint32_t uuid,
                                             struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;

    err = sst_read_encrypted_object(uuid, obj);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_encrypted_object_write(uint32_t uuid,
                                              struct sst_object_t *obj)
{
    enum tfm_sst_err_t err;

    /* Encrypt the object data */
    err = sst_object_auth_encrypt(obj->header.info.size_current, obj);
    if (err != TFM_SST_ERR_SUCCESS) {
        return err;
    }

    /* Write encrypted object to the object system */
    err = sst_core_object_write(uuid, (uint8_t *)obj,
                                SST_OBJECT_START_POSITION,
                                SST_ENCRYPTED_SIZE(
                                                obj->header.info.size_current));

    return err;
}
