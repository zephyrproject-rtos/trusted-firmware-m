/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_ATTEST_TEST_SERVICE_API_H__
#define __TFM_ATTEST_TEST_SERVICE_API_H__

#include <stdint.h>
#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the initial attestation public key.
 *
 * \param[out]  public_key        Pointer to the buffer where the public key
 *                                will be stored.
 * \param[in]   key_buf_size      Size of allocated buffer for key, in bytes.
 * \param[out]  public_key_len    Size of public key in bytes.
 * \param[out]  public_key_curve  Type of the elliptic curve which the key
 *                                belongs to.
 *
 * \note Currently only the ECDSA P-256 over SHA-256 algorithm is supported.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t tfm_initial_attest_get_public_key(uint8_t *public_key,
                                               size_t  public_key_buf_size,
                                               size_t  *public_key_len,
                                               psa_ecc_family_t *curve_type);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_ATTEST_TEST_SERVICE_API_H__ */
