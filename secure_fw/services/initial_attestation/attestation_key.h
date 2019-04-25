/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_KEY_H__
#define __ATTESTATION_KEY_H__

#include "psa_initial_attestation_api.h"
#include "psa_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the initial attestation key from platform layer and register it
 *        to crypto service for further usage (signing or verification).
 *
 * \param[out] key_handle_private Pointer to the key handle allocated for the
 *                                private key
 * \param[out] key_handle_public  Pointer to the key handle allocated for the
 *                                public key
 *
 * Private key MUST be present on the device, public key is optional.
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key(s) was registered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key(s) could not be registered.
 */
enum psa_attest_err_t attest_register_initial_attestation_key(
                                           psa_key_handle_t *key_handle_private,
                                           psa_key_handle_t *key_handle_public);
/**
 * \brief Unregister the initial attestation key(s) from crypto service to do
 *        not occupy key slot(s).
 *
 * \param[in] key_handle_private Key handle associated to the private key
 * \param[in] key_handle_public  Key handle associated to the public key
 *
 * \retval  PSA_ATTEST_ERR_SUCCESS   Key(s) was unregistered.
 * \retval  PSA_ATTEST_ERR_GENERAL   Key(s) could not be unregistered.
 */
enum psa_attest_err_t attest_unregister_initial_attestation_key(
                                            psa_key_handle_t key_handle_private,
                                            psa_key_handle_t key_handle_public);
#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_KEY_H__ */
