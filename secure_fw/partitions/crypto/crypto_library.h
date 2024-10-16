/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * @file crypto_library.h
 *
 * @brief This file contains some abstractions required to interface the
 *        TF-M Crypto service to an underlying cryptographic library that
 *        implements the PSA Crypto API. The TF-M Crypto service uses this
 *        library to provide a PSA Crypto core layer implementation and
 *        a software or hardware based implementation of crypto algorithms.
 */

#ifndef CRYPTO_LIBRARY_H
#define CRYPTO_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief Some integration might decide to enforce the same ABI on client and
 *        service interfaces to PSA Crypto defining the \a CRYPTO_LIBRARY_ABI_COMPAT
 *        In this case the size of the structure describing the key attributes
 *        is the same both in client and server views. The semantics remain
 *        unchanged
 */
#if defined(CRYPTO_LIBRARY_ABI_COMPAT) && (CRYPTO_LIBRARY_ABI_COMPAT == 1)
#define TFM_CRYPTO_KEY_ATTR_OFFSET_CLIENT_SERVER (0)
#else
#define TFM_CRYPTO_KEY_ATTR_OFFSET_CLIENT_SERVER (sizeof(mbedtls_key_owner_id_t))
#endif /* CRYPTO_LIBRARY_ABI_COMPAT */

/**
 * @brief This macro extracts the key ID from the library encoded key passed as parameter
 *
 */
#define CRYPTO_LIBRARY_GET_KEY_ID(encoded_key_library) MBEDTLS_SVC_KEY_ID_GET_KEY_ID(encoded_key_library)

/**
 * @brief This macro extracts the owner from the library encoded key passed as parameter
 *
 */
#define CRYPTO_LIBRARY_GET_OWNER(encoded_key_library) MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(encoded_key_library)

/**
 * @brief The following typedef must be defined to the type associated to the key_id in the underlying library
 *
 */
typedef mbedtls_svc_key_id_t tfm_crypto_library_key_id_t;

/**
 * @brief Function used to initialise an object of \ref tfm_crypto_library_key_id_t to a (owner, key_id) pair
 *
 * @param[in] owner   Owner of the key
 * @param[in] key_id  key ID associated to the key of type \ref psa_key_id_t
 *
 * @return An object of type \ref tfm_crypto_library_key_id_t
 *
 */
tfm_crypto_library_key_id_t tfm_crypto_library_key_id_init(int32_t owner, psa_key_id_t key_id);

/**
 * @brief This function is used to retrieve a string describing the library used in the backend
 *        to provide information to the crypto service and the user
 *
 * @return A NULL terminated string describing the backend library
 */
char *tfm_crypto_library_get_info(void);

/**
 * @brief This function initialises a \ref tfm_crypto_library_key_id_t with default values
 *
 */
static inline tfm_crypto_library_key_id_t tfm_crypto_library_key_id_init_default(void)
{
    return tfm_crypto_library_key_id_init(0, 0);
}

/**
 * @brief Allows to set the owner of a library key embedded into the key attributes structure
 *
 * @param[in]  owner The owner value to be written into the key attributes structure
 * @param[out] attr  Pointer to the key attributes into which we want to e
 *
 */
void tfm_crypto_library_get_library_key_id_set_owner(int32_t owner, psa_key_attributes_t *attr);

/*!
 * @brief This function is used to perform the necessary steps to initialise the underlying
 *        library that provides the implementation of the PSA Crypto core to the TF-M Crypto
 *        service
 *
 * @return PSA_SUCCESS on successful initialisation
 */
psa_status_t tfm_crypto_core_library_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_LIBRARY_H */
