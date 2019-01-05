/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_H__
#define __ATTESTATION_H__

#include "psa_initial_attestation_api.h"
#include "psa_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Extension of shared data TLVs, defined in bl2/include/tfm_boot_status.h */
/* #define GENERAL_BOOT_SEED          0x00 */ /* Defined in tfm_boot_status.h */
#define GENERAL_INSTANCE_ID        0x01
#define GENERAL_CHALLENGE          0x02
#define GENERAL_CALLER_ID          0x03
#define GENERAL_IMPLEMENTATION_ID  0x04
#define GENERAL_HW_VERSION         0x05
#define GENERAL_SECURITY_LIFECYCLE 0x06

#define TLV_MINOR_IAS_INSTANCE_ID \
                            ((SW_GENERAL << 6) | GENERAL_INSTANCE_ID)
#define TLV_MINOR_IAS_CHALLENGE \
                            ((SW_GENERAL << 6) | GENERAL_CHALLENGE)
#define TLV_MINOR_IAS_CALLER_ID \
                            ((SW_GENERAL << 6) | GENERAL_CALLER_ID)
#define TLV_MINOR_IAS_IMPLEMENTATION_ID \
                            ((SW_GENERAL << 6) | GENERAL_IMPLEMENTATION_ID)
#define TLV_MINOR_IAS_HW_VERSION \
                            ((SW_GENERAL << 6) | GENERAL_HW_VERSION)
#define TLV_MINOR_IAS_SECURITY_LIFECYCLE \
                            ((SW_GENERAL << 6) | GENERAL_SECURITY_LIFECYCLE)
#define TLV_MINOR_IAS_IAT_VERSION \
                            ((SW_GENERAL << 6) | GENERAL_IAT_VERSION)

/*!
 * \brief Initialise the initial attestation service during the TF-M boot up
 *        process.
 *
 * \return Returns PSA_ATTEST_ERR_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t attest_init(void);

/*!
 * \brief Get initial attestation token
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input data
 *                           to attestation service
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in/out] out_vec    Pointer out_vec array, which contains output data
 *                           to attestation service
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
initial_attest_get_token(const psa_invec  *in_vec,  uint32_t num_invec,
                               psa_outvec *out_vec, uint32_t num_outvec);

/**
 * \brief Get the size of the initial attestation token
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input data
 *                           to attestation service
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[out]    out_vec    Pointer to out_vec array, which contains pointer
 *                           where to store the output data
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
initial_attest_get_token_size(const psa_invec  *in_vec,  uint32_t num_invec,
                                    psa_outvec *out_vec, uint32_t num_outvec);
#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_H__ */
