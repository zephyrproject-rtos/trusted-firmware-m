/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "psa_initial_attestation_api.h"
#include "attestation.h"
#include "secure_utilities.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "psa_client.h"
#include "bl2/include/tfm_boot_status.h"
#include "platform/include/tfm_plat_defs.h"
#include "platform/include/tfm_plat_device_id.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "tfm_attest_hal.h"

#define MAX_BOOT_STATUS 512

/*!
 * \var boot_status
 *
 * \brief Array variable to store the boot status in service's memory.
 *
 * \details Boot status comes from the secure bootloader and primarily stored
 *          on a memory area which is shared between bootloader and SPM.
 *          SPM provides the \ref tfm_core_get_boot_data() API to retrieve
 *          the service related data from shared area.
 */

/* FixMe: Enforcement of 4 byte alignment can be removed as soon as memory type
 *        is configured in the MPU to be normal, instead of device, which
 *        prohibits unaligned access.
 */
__attribute__ ((aligned(4)))
static uint8_t boot_status[MAX_BOOT_STATUS];

enum psa_attest_err_t attest_init(void)
{
    enum tfm_status_e res;

    res = tfm_core_get_boot_data(TLV_MAJOR_IAS, boot_status, MAX_BOOT_STATUS);
    if (res != TFM_SUCCESS) {
        return PSA_ATTEST_ERR_INIT_FAILED;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to look up a specific entry in the shared data
 *        section.
 *
 * \param[in]  minor_type The identifier of the shared data entry
 * \param[out] tlv_len    Length of the shared data entry
 * \param[out] tlv_ptr    Pointer to the shared data entry
 *
 * \return Returns 0 on success. Otherwise, 1.
 */
static uint32_t attest_get_tlv(uint16_t   minor_type,
                               uint16_t  *tlv_len,
                               uint8_t  **tlv_ptr)
{
    struct shared_data_tlv_header *tlv_header;
    struct shared_data_tlv_entry  *tlv_entry;
    uintptr_t tlv_end;
    uintptr_t tlv_curr;

    tlv_header = (struct shared_data_tlv_header *)boot_status;
    if (tlv_header->tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return 1;
    }

    /* Get the boundaries of TLV section */
    tlv_end  = (uintptr_t)boot_status + tlv_header->tlv_tot_len;
    tlv_curr = (uintptr_t)boot_status + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section and copy TLVs with requested minor
     * type to the provided buffer.
     */
    for(; tlv_curr < tlv_end; tlv_curr += tlv_entry->tlv_len) {
        tlv_entry = (struct shared_data_tlv_entry *)tlv_curr;
        if (GET_MINOR(tlv_entry->tlv_type) == minor_type) {
            *tlv_ptr = (uint8_t *)tlv_entry;
            *tlv_len = tlv_entry ->tlv_len;
            return 0;
        }
    }

    return 1;
}

/*!
 * \brief Static function to copy a TLV entry from shared data section to the
 *        attestation token.
 *
 * \param[in]  tlv_len        The length of TLV entry in bytes
 * \param[in]  tlv_ptr        Pointer from where to copy the TLV entry
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns 0 on success. Otherwise, 1.
 */
static uint32_t attest_copy_tlv(uint16_t       tlv_len,
                                const uint8_t *tlv_ptr,
                                uint32_t       token_buf_size,
                                uint8_t       *token_buf)
{
    struct shared_data_tlv_header *tlv_header;
    uint8_t *next_tlv = token_buf;

    tlv_header = (struct shared_data_tlv_header *)token_buf;
    if (tlv_header->tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return 1;
    }

    if (tlv_header->tlv_tot_len + tlv_len > token_buf_size) {
        return 1;
    }

    next_tlv += tlv_header->tlv_tot_len;
    tlv_header->tlv_tot_len += tlv_len;
    tfm_memcpy(next_tlv, tlv_ptr, tlv_len);

    return 0;
}

/*!
 * \brief Static function to add a TLV entry to the attestation token.
 *
 * \param[in]  minor_type     The identifier of the TLV entry
 * \param[in]  size           Size of the TLV entry in bytes
 * \param[in]  data           Pointer to the buffer which stores the TLV entry
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns 0 on success. Otherwise, 1.
 */
static uint32_t attest_add_tlv(uint16_t       minor_type,
                               uint32_t       size,
                               const uint8_t *data,
                               uint32_t       token_buf_size,
                               uint8_t       *token_buf)
{
    struct shared_data_tlv_header *tlv_header;
    struct shared_data_tlv_entry  *tlv_entry;
    uint8_t *next_tlv = token_buf;

    tlv_header = (struct shared_data_tlv_header *)token_buf;
    if (tlv_header->tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return 1;
    }

    if (tlv_header->tlv_tot_len + SHARED_DATA_ENTRY_SIZE(size) >
        token_buf_size) {
        return 1;
    }

    next_tlv += tlv_header->tlv_tot_len;
    tlv_header->tlv_tot_len += SHARED_DATA_ENTRY_SIZE(size);

    tlv_entry = (struct shared_data_tlv_entry *)next_tlv;
    tlv_entry->tlv_type = SET_TLV_TYPE(TLV_MAJOR_IAS, minor_type);
    tlv_entry->tlv_len = SHARED_DATA_ENTRY_SIZE(size);

    next_tlv += SHARED_DATA_ENTRY_HEADER_SIZE;
    tfm_memcpy(next_tlv, data, size);

    return 0;
}

/*!
 * \brief Static function to initalise the token buffer. Add TLV data header to
 *        it.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_init_token(uint32_t token_buf_size, uint8_t *token_buf)
{
    struct shared_data_tlv_header *tlv_header;

    if (SHARED_DATA_HEADER_SIZE > token_buf_size) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    tlv_header = (struct shared_data_tlv_header *)token_buf;
    tlv_header->tlv_magic = SHARED_DATA_TLV_INFO_MAGIC;
    tlv_header->tlv_tot_len = SHARED_DATA_HEADER_SIZE;

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add boot status claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_s_ns_sha256_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    uint16_t tlv_len;
    uint8_t *tlv_ptr;
    uint32_t res;

    res = attest_get_tlv(TLV_MINOR_IAS_S_NS_MEASURE_VALUE, &tlv_len, &tlv_ptr);
    if (res != 0) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_copy_tlv(tlv_len, tlv_ptr, token_buf_size, token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add boot seed claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_boot_seed_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    /* FixMe: Enforcement of 4 byte alignment can be removed as soon as memory
     *        type is configured in the MPU to be normal, instead of device,
     *        which prohibits unaligned access.
     */
    __attribute__ ((aligned(4)))
    uint8_t boot_seed[BOOT_SEED_SIZE];
    enum tfm_plat_err_t res;

    res = tfm_plat_get_boot_seed(sizeof(boot_seed), boot_seed);
    if (res != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_add_tlv(TLV_MINOR_IAS_BOOT_SEED,
                         BOOT_SEED_SIZE,
                         boot_seed,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/* FixMe: Remove this #if when MPU will be configured properly. Currently
 *        in case of TFM_LVL == 3 unaligned access triggers a usage fault
 *        exception.
 */
#if !defined(TFM_LVL) || (TFM_LVL == 1)
/*!
 * \brief Static function to add instance id claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_instance_id_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    /* FixMe: Enforcement of 4 byte alignment can be removed as soon as memory
     *        type is configured in the MPU to be normal, instead of device,
     *        which prohibits unaligned access.
     */
    __attribute__ ((aligned(4)))
    uint8_t instance_id[INSTANCE_ID_MAX_SIZE];
    uint32_t res;
    enum tfm_plat_err_t res_plat;
    uint32_t size = sizeof(instance_id);

    res_plat = tfm_plat_get_instance_id(&size, instance_id);
    if (res_plat != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_add_tlv(TLV_MINOR_IAS_INSTANCE_ID,
                         size,
                         instance_id,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add implementation id claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_implementation_id_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    /* FixMe: Enforcement of 4 byte alignment can be removed as soon as memory
     *        type is configured in the MPU to be normal, instead of device,
     *        which prohibits unaligned access.
     */
    __attribute__ ((aligned(4)))
    uint8_t implementation_id[IMPLEMENTATION_ID_MAX_SIZE];
    uint32_t res;
    enum tfm_plat_err_t res_plat;
    uint32_t size = sizeof(implementation_id);

    res_plat = tfm_plat_get_implementation_id(&size, implementation_id);
    if (res_plat != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_add_tlv(TLV_MINOR_IAS_IMPLEMENTATION_ID,
                         size,
                         implementation_id,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add hardware version claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_hw_version_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    /* FixMe: Enforcement of 4 byte alignment can be removed as soon as memory
     *        type is configured in the MPU to be normal, instead of device,
     *        which prohibits unaligned access.
     */
    __attribute__ ((aligned(4)))
    uint8_t hw_version[HW_VERSION_MAX_SIZE];
    uint32_t res;
    enum tfm_plat_err_t res_plat;
    uint32_t size = sizeof(hw_version);

    res_plat = tfm_plat_get_hw_version(&size, hw_version);
    if (res_plat != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_add_tlv(TLV_MINOR_IAS_HW_VERSION,
                         size,
                         hw_version,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}
#endif

/*!
 * \brief Static function to add caller id claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_caller_id_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    uint32_t res;
    int32_t  caller_id;

    res = tfm_core_get_caller_client_id(&caller_id);
    if (res != 0) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    res = attest_add_tlv(TLV_MINOR_IAS_CALLER_ID,
                         sizeof(int32_t),
                         (uint8_t *)&caller_id,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add security lifecycle claim to attestation token.
 *
 * \param[in]  token_buf_size Size of token buffer in bytes
 * \param[out] token_buf      Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_security_lifecycle_claim(uint32_t token_buf_size, uint8_t *token_buf)
{
    uint32_t res;
    enum tfm_security_lifecycle_t security_lifecycle;

    security_lifecycle = tfm_attest_hal_get_security_lifecycle();

    res = attest_add_tlv(TLV_MINOR_IAS_SECURITY_LIFECYCLE,
                         sizeof(enum tfm_security_lifecycle_t),
                         (uint8_t *)&security_lifecycle,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to add challenge claim to attestation token.
 *
 * \param[in]  challenge_buf_size Size of challenge object in bytes
 * \param[in]  challenge_buf      Pointer to buffer which stores the challenge
 *                                object
 * \param[in]  token_buf_size     Size of token buffer in bytes
 * \param[out] token_buf          Pointer to buffer which stores the token
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t
attest_add_challenge_claim(uint32_t       challenge_buf_size,
                           const uint8_t *challenge_buf,
                           uint32_t       token_buf_size,
                           uint8_t       *token_buf)
{
    uint32_t res;

    res = attest_add_tlv(TLV_MINOR_IAS_CHALLENGE,
                         challenge_buf_size,
                         challenge_buf,
                         token_buf_size,
                         token_buf);
    if (res != 0) {
        return PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

/*!
 * \brief Static function to retrieve the constructed attestation token's size.
 *
 * \param[in] token_buf Pointer to buffer which stores the token
 *
 * \return Returns the size of token in bytes
 */
static uint32_t attest_get_token_size(const uint8_t *token_buf)
{
    struct shared_data_tlv_header *tlv_header;

    tlv_header = (struct shared_data_tlv_header *)token_buf;

    return tlv_header->tlv_tot_len;
}

/* Initial implementation of attestation service:
 *  - data is TLV encoded
 *  - token is not signed yet
 *  - only fixed set of claims are supported
 *  - external claims are not handled, expect challenge object
 */
enum psa_attest_err_t
initial_attest_get_token(const psa_invec  *in_vec,  uint32_t num_invec,
                               psa_outvec *out_vec, uint32_t num_outvec)
{
    enum tfm_status_e tfm_err;
    enum psa_attest_err_t attest_err = PSA_ATTEST_ERR_SUCCESS;

    const uint8_t *challenge_buf = (uint8_t *)in_vec[0].base;
    size_t   challenge_buf_size  = in_vec[0].len;
    uint8_t *token_buf           = (uint8_t *)out_vec[0].base;
    size_t  *token_buf_size      = &(out_vec[0].len);

    if (challenge_buf_size > PSA_INITIAL_ATTEST_MAX_CHALLENGE_SIZE) {
        return PSA_ATTEST_ERR_INVALID_INPUT;
    }

    if (challenge_buf_size > 0) {
        tfm_err = tfm_core_memory_permission_check((void *)challenge_buf,
                                                   challenge_buf_size,
                                                   TFM_MEMORY_ACCESS_RO);
        if (tfm_err != TFM_SUCCESS) {
            attest_err =  PSA_ATTEST_ERR_INVALID_INPUT;
            goto error;
        }
    }

    tfm_err = tfm_core_memory_permission_check(token_buf,
                                               *token_buf_size,
                                               TFM_MEMORY_ACCESS_RW);
    if (tfm_err != TFM_SUCCESS) {
        attest_err =  PSA_ATTEST_ERR_INVALID_INPUT;
        goto error;
    }

    attest_err = attest_init_token(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    attest_err = attest_add_s_ns_sha256_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    attest_err = attest_add_boot_seed_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    /* FixMe: Remove this #if when MPU will be configured properly. Currently
     *        in case of TFM_LVL == 3 unaligned access triggers a usage fault
     *        exception.
     */
#if !defined(TFM_LVL) || (TFM_LVL == 1)
    attest_err = attest_add_instance_id_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    attest_err = attest_add_hw_version_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    attest_err = attest_add_implementation_id_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }
#endif

    if (challenge_buf_size > 0) {
        attest_err = attest_add_challenge_claim(challenge_buf_size,
                                                challenge_buf,
                                                *token_buf_size,
                                                token_buf);
        if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
            goto error;
        }
    }

    attest_err = attest_add_caller_id_claim(*token_buf_size, token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

    attest_err = attest_add_security_lifecycle_claim(*token_buf_size,
                                                     token_buf);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS) {
        goto error;
    }

     /* FixMe: Token should be signed with attestation key */

    *token_buf_size = attest_get_token_size(token_buf);

error:
    return attest_err;
}

/* Initial implementation, just returns with hard coded value */
enum psa_attest_err_t
initial_attest_get_token_size(const psa_invec  *in_vec,  uint32_t num_invec,
                                    psa_outvec *out_vec, uint32_t num_outvec)
{
    uint32_t *token_buf_size = (uint32_t *)out_vec[0].base;

    if (out_vec[0].len < sizeof(uint32_t)) {
        return PSA_ATTEST_ERR_INVALID_INPUT;
    }

    *token_buf_size = PSA_INITIAL_ATTEST_TOKEN_SIZE;

    return PSA_ATTEST_ERR_SUCCESS;
}
