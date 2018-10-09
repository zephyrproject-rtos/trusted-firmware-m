/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CRYPTO_ENGINE_H__
#define __CRYPTO_ENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include "psa_crypto.h"

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
/* Pre include Mbed TLS headers */
#define LIB_PREFIX_NAME __tfm_crypto__
#include "mbedtls_global_symbols.h"

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files. */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

/**
 * \brief List of engine specific includes for Mbed TLS
 *
 */
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/md.h"
#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"

#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

/**
 * \brief Generic engine cipher context type
 *
 */
union engine_cipher_context {
    uint32_t dummy; /* Needed to keep the union always not empty */
#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    mbedtls_cipher_context_t ctx;
#endif
};

/**
 * \brief Generic engine hash context type
 *
 */
union engine_hash_context {
    uint32_t dummy; /* Needed to keep the union always not empty */
#if defined (TFM_CRYPTO_ENGINE_MBEDTLS)
    mbedtls_md_context_t ctx;
#endif
};

/**
 * \brief Generic engine cmac context type
 *
 */
union engine_cmac_context {
    uint32_t dummy; /* Needed to keep the union always not empty */
#if defined (TFM_CRYPTO_ENGINE_MBEDTLS)
    mbedtls_cmac_context_t ctx;
#endif
};



/**
 * \brief For a cipher operation, define the possible modes of configuration.
 */
enum engine_cipher_mode_t {
    ENGINE_CIPHER_MODE_NONE = 0, /*!< Cipher mode not selected */
    ENGINE_CIPHER_MODE_DECRYPT,  /*!< Cipher mode set to decryption */
    ENGINE_CIPHER_MODE_ENCRYPT,  /*!< Cipher mode set to encryption */
    /* Used to force enum size */
    ENGINE_CIPHER_MODE_FORCE_INT_SIZE = INT_MAX
};

/**
 * \brief Structure used to keep engine information during the engine setup
 *        step for a hash operation.
 */
struct hash_engine_info {
    uint32_t type;           /*!< Engine specific identifier which describes
                              *   the operation which has to be configured on
                              *   the crypto engine
                              */
};

/**
 * \brief Structure used to keep engine information during the engine setup
 *        step for a cipher operation
 */
struct cipher_engine_info {
    uint32_t type;           /*!< Engine specific identifier which describes
                              *   the operation which has to be configured on
                              *   the crypto engine
                              */
    enum engine_cipher_mode_t cipher_mode; /*!< Describes if the cipher on
                                            *   the engine is configured for
                                            *   encryption or decryption
                                            */
    uint32_t padding_mode; /*!< Engine specific identifier which describes
                            *   the padding mode which has been configured
                            *   on the cipher, if any
                            */
};

/**
 * \brief This function performs all the generic operations required to
 *        initialise the crypto engine
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_init(void);

/**
 * \brief This function performs the setup of a multipart hash operation on the
 *        crypto engine
 *
 * \param[in]  alg         Algorithm to be setup
 * \param[out] engine_info Pointer to the engine_info structure containing
 *                         engine parameters determined during the setup
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_hash_setup(const psa_algorithm_t alg,
                                          struct hash_engine_info *engine_info);
/**
 * \brief This function starts a multipart hash operation on the crypto engine
 *
 * \param[in/out] hp          Pointer to the hash engine context to be used
 * \param[in]     engine_info Pointer to the engine_info structure as determined
 *                            during the setup step
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_hash_start(union engine_hash_context *hp,
                                    const struct hash_engine_info *engine_info);
/**
 * \brief This function updates a multipart hash operation with one chunk of
 *        input data
 *
 * \param[in/out] hp           Pointer to the hash engine context to be used
 * \param[in]     input        Pointer to the buffer containing the input data
 * \param[in]     input_length Size in bytes of the input data
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_hash_update(union engine_hash_context *hp,
                                           const uint8_t *input,
                                           const uint32_t input_length);
/**
 * \brief This function finalises a multipart hash operation producing one hash
 *        value in output as described by the operation context
 *
 * \param[in/out] hp   Pointer to the hash engine context to be used
 * \param[out]    hash Pointer to the buffer containing the output hash value
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_hash_finish(union engine_hash_context *hp,
                                           uint8_t *hash);
/**
 * \brief This function releases the crypto engine resources associated to a
 *        multipart hash operation context
 *
 * \param[in/out] hp Pointer to the hash engine context to be used
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_hash_release(union engine_hash_context *hp);

/**
 * \brief This function performs the setup of a multipart cipher operation on
 *        the crypto engine
 *
 * \param[in] alg                Algorithm to be configured
 * \param[in] key_type           Key type of the key that will be used
 * \param[in] key_size           Size in bits of the key that will be used
 * \param[in] engine_cipher_mode Parameter specifying if the cipher must be set
 *                               in encrypt or decrypt mode
 * \param[out] engine_info       Pointer to the engine configuration structure
 *                               containing engine parameters determined during
 *                               setup
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_setup(const psa_algorithm_t alg,
                             const psa_key_type_t key_type,
                             const uint32_t key_size,
                             const enum engine_cipher_mode_t engine_cipher_mode,
                             struct cipher_engine_info *engine_info);
/**
 * \brief This function sets the padding mode on the crypto engine based on the
 *        information gathered during the setup phase
 *
 * \param[in/out] cp          Pointer to the cipher engine context to be used
 * \param[in]     engine_info Pointer to the engine configuration structure
 *                            containing engine parameters determined during
 *                            setup
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_set_padding_mode(
                                  union engine_cipher_context *cp,
                                  const struct cipher_engine_info *engine_info);
/**
 * \brief This function starts a multipart cipher operation
 *
 * \param[in/out] cp          Pointer to the cipher engine context to be used
 * \param[in]     engine_info Pointer to the engine configuration structure
 *                            containing engine parameters determined during
 *                            setup
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_start(
                                  union engine_cipher_context *cp,
                                  const struct cipher_engine_info *engine_info);
/**
 * \brief This function sets the key data on the crypto engine for a multipart
 *        cipher operation. It reads also from the engine_info configuration
 *        item to determine if the key needs to be set in encryption or
 *        decryption mode on the engine.
 *
 * \param[in/out] cp          Pointer to the cipher engine context to be used
 * \param[in]     key_data    Pointer to the buffer containing key material
 * \param[in]     key_size    Size in bytes of the key pointed by key_data
 * \param[in]     engine_info Pointer to the engine configuration structure
 *                            containing engine parameters determined during
 *                            setup
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_set_key(
                                  union engine_cipher_context *cp,
                                  const uint8_t *key_data,
                                  const uint32_t key_size,
                                  const struct cipher_engine_info *engine_info);
/**
 * \brief This function sets the initialisation vector on the crypto engine for
 *        a multipart cipher operation
 *
 * \param[in/out] cp        Pointer to the cipher engine context to be used
 * \param[in]     iv        Pointer to the buffer containing the IV
 * \param[in]     iv_length Size in bytes of the initialisation vector
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_set_iv(union engine_cipher_context *cp,
                                             const uint8_t *iv,
                                             const uint32_t iv_length);
/**
 * \brief This function updates a multipart cipher operation on the crypto
 *        engine with a new chunk of input data. It may produce output data.
 *
 * \param[in/out] cp            Pointer to the cipher engine context to be used
 * \param[in]     input         Pointer to the buffer containing the input data
 *                              chunk
 * \param[in]     input_length  Size in bytes of the input data chunk
 * \param[out]    output        Pointer to the buffer containing the output data
 * \param[out]    output_length Pointer to the size in bytes of the data produced
 *                              as output
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_update(union engine_cipher_context *cp,
                                             const uint8_t *input,
                                             const uint32_t input_length,
                                             uint8_t *output,
                                             uint32_t *output_length);
/**
 * \brief This function finalises a multipart cipher operation on the crypto
 *        engine. It may produce output data.
 *
 * \param[in/out] cp            Pointer to the cipher engine context to be used
 * \param[out]    output        Pointer to the buffer containing the output data
 * \param[out]    output_length Pointer to the size in bytes of the data produced
 *                              as output
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_finish(union engine_cipher_context *cp,
                                             uint8_t *output,
                                             uint32_t *output_length);
/**
 * \brief This function releases the crypto engine resources associated to a
 *        multipart cipher operation context
 *
 * \param[in/out] cp Pointer to the cipher engine context to be used
 *
 * \return Return values as specified by \ref psa_status_t
 */
psa_status_t tfm_crypto_engine_cipher_release(union engine_cipher_context *cp);
#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_ENGINE_H__ */
