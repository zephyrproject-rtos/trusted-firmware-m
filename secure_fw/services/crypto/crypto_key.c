/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

/* Pre include Mbed TLS headers */
#define LIB_PREFIX_NAME __tfm_crypto__
#include "mbedtls_global_symbols.h"

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files.
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "psa_crypto.h"

/* The file "psa_crypto_struct.h" contains definitions for
 * implementation-specific structs that are declared in "psa_crypto.h".
 */
#include "psa_crypto_struct.h"

/**
 * \brief This value defines the maximum number of simultaneous key stores
 *        supported by this implementation.
 */
#define TFM_CRYPTO_KEY_STORAGE_NUM (4)

struct tfm_crypto_key_storage_s {
    uint8_t in_use;                 /*!< Indicates if the key store is in use */
    psa_key_type_t type;            /*!< Type of the key stored */
    size_t data_length;             /*!< Length of the key stored */
    uint8_t data[TFM_CRYPTO_MAX_KEY_LENGTH]; /*!< Buffer containining the key */
};

static struct tfm_crypto_key_storage_s
                                key_storage[TFM_CRYPTO_KEY_STORAGE_NUM] = {{0}};
/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_import_key(psa_key_slot_t key,
                                            psa_key_type_t type,
                                            const uint8_t *data,
                                            size_t data_length)
{
    size_t i;

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use != TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_OCCUPIED_SLOT;
    }

    if (data_length > TFM_CRYPTO_MAX_KEY_LENGTH) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    key_storage[key].in_use = TFM_CRYPTO_IN_USE;
    key_storage[key].type = type;

    for (i=0; i<data_length; i++) {
        key_storage[key].data[i] = data[i];
    }

    key_storage[key].data_length = data_length;


    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_destroy_key(psa_key_slot_t key)
{
    uint32_t i;

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    volatile uint8_t *p_mem = (uint8_t *) &(key_storage[key]);
    uint32_t size_mem = sizeof(struct tfm_crypto_key_storage_s);

    /* memset the key_storage */
    for (i=0; i<size_mem; i++) {
        p_mem[i] = 0;
    }

    /* Set default values */
    key_storage[key].in_use = TFM_CRYPTO_NOT_IN_USE;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_get_key_information(psa_key_slot_t key,
                                                     psa_key_type_t *type,
                                                     size_t *bits)
{
    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use == TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_EMPTY_SLOT;
    }

    /* Get basic metadata */
    *type = key_storage[key].type;
    *bits = PSA_BYTES_TO_BITS(key_storage[key].data_length);

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_export_key(psa_key_slot_t key,
                                            uint8_t *data,
                                            size_t data_size,
                                            size_t *data_length)
{
    size_t i;

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use == TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_EMPTY_SLOT;
    }

    if (key_storage[key].data_length > data_size) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    for (i=0; i<key_storage[key].data_length; i++) {
        data[i] = key_storage[key].data[i];
    }

    *data_length = key_storage[key].data_length;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_export_public_key(psa_key_slot_t key,
                                                   uint8_t *data,
                                                   size_t data_size,
                                                   size_t *data_length)
{
    /* FIXME: This API is not supported yet */
    return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
}
/*!@}*/
