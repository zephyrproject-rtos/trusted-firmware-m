/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/***************************************************************************/
/*                             DRAFT UNDER REVIEW                          */
/*   These APIs are still evolving and are meant as a prototype for review.*/
/*   The APIs will change depending on feedback and will be firmed up      */
/*   to a stable set of APIs once all the feedback has been considered.    */
/***************************************************************************/

#ifndef __PSA_INITIAL_ATTESTATION_API_H__
#define __PSA_INITIAL_ATTESTATION_API_H__

#include "tfm_api.h"
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief PSA INITIAL ATTESTATION API version
 */
#define PSA_INITIAL_ATTEST_API_VERSION_MAJOR (0)
#define PSA_INITIAL_ATTEST_API_VERSION_MINOR (1)

/* The return value is shared with the TFM partition status value. The ATTEST
 * return codes shouldn't overlap with predefined TFM status values.
 */
#define PSA_ATTEST_ERR_OFFSET (TFM_PARTITION_SPECIFIC_ERROR_MIN)

/**
 * \enum psa_attest_err_t
 *
 * \brief Initial attestation service error types
 *
 */
enum psa_attest_err_t {
    PSA_ATTEST_ERR_SUCCESS = 0,
    PSA_ATTEST_ERR_INIT_FAILED = PSA_ATTEST_ERR_OFFSET,
    PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW,
    PSA_ATTEST_ERR_CLAIM_UNAVAILABLE,
    PSA_ATTEST_ERR_INVALID_INPUT,
    PSA_ATTEST_ERR_GENERAL,
    /* Following entry is only to ensure the error code of int size */
    PSA_ATTEST_ERR_FORCE_INT_SIZE = INT_MAX
};

/**
 * Maximum size of input challenge in bytes. This can be a nonce from server or
 * the hash of any data which must be attested to the server.
 */
#define PSA_INITIAL_ATTEST_MAX_CHALLENGE_SIZE (64u)

/**
 * The list of fixed claims in the initial attestation token is still evolving,
 * you can expect slight changes in the future.
 *
 * The initial attestation token is planned to be aligned with later version of
 * Entity Attestation Token format:
 * https://tools.ietf.org/html/draft-mandyam-eat-00
 *
 * Current list of claims:
 *  - Challenge:   Input object from caller. Can be nonce from server or hash of
 *                 attested data.
 *  - Client ID:   The ID of that secure partition or non-secure thread who
 *                 called the initial attestation API.
 *  - Boot status: Measurements(hash) of firmware components by secure
 *                 bootloader.
 *  - Boot seed:   Random number, which is constant during the same boot cycle.
 *  - Device ID:   Universally and globally unique ID of the device.
 *  - HW version:  Uniquely identifies the GDSII that went to fabrication, HW
 *                 and ROM.
 */

/**
 * Calculated based on:
 *  - COSE header
 *    - Algorithm identifier
 *    - Key ID
 *  - Claims in initial attestation token:
 *  - COSE Signature
 *
 * This size (in bytes) is a maximum value, actual token size can be smaller.
 */
/* FixMe: Just initial value it must be updated if claims are fixed */
#define PSA_INITIAL_ATTEST_TOKEN_SIZE (512u)

/**
 * \brief Get initial attestation token
 *
 * \param[in]     challenge_obj   Pointer to buffer where challenge input is
 *                                stored. Nonce and / or hash of attested data.
 *                                Must be always
 *                                \ref PSA_INITIAL_ATTEST_CHALLENGE_SIZE bytes
 *                                long.
 * \param[in]     challenge_size  Size of challenge object in bytes.
 * \param[out]    token           Pointer to the buffer where attestation token
 *                                must be stored.
 * \param[in/out] token_size      Size of allocated buffer for token, which
 *                                updated by initial attestation service with
 *                                final token size.
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
psa_initial_attest_get_token(const uint8_t *challenge_obj,
                             uint32_t       challenge_size,
                             uint8_t       *token,
                             uint32_t      *token_size);

#ifdef __cplusplus
}
#endif

#endif /* __PSA_INITIAL_ATTESTATION_API_H__ */
