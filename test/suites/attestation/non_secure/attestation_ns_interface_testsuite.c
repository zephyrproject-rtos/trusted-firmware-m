/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_ns_tests.h"
#include "psa_initial_attestation_api.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "bl2/include/tfm_boot_status.h"
#include "secure_utilities.h"
#include "platform/include/tfm_plat_device_id.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "tfm_api.h"
#include <string.h>
#include <stdlib.h>

static uint8_t token_buffer[TEST_TOKEN_SIZE];
static uint8_t challenge_buffer[TEST_CHALLENGE_OBJ_SIZE];

/* Define test suite for attestation service tests */
/* List of tests */
static void tfm_attest_test_1001(struct test_result_t *ret);
static void tfm_attest_test_1002(struct test_result_t *ret);

static struct test_t attestation_interface_tests[] = {
    {&tfm_attest_test_1001, "TFM_ATTEST_TEST_1001",
     "Get attestation token and check claims", {0} },
    {&tfm_attest_test_1002, "TFM_ATTEST_TEST_1002",
     "Negative test cases for initial attestation service", {0} },
};

/*!
 * \brief Static function to iterates over the TLV fields in the attestation
 *        token and look for the location of the specified TLV type.
 *
 * \param[in]  minor_type The identifier of the TLV data entry
 * \param[in]  token_buf  Pointer to the buffer which stores the token
 * \param[out] tlv_ptr    Pointer to the TLV data entry
 *
 * \return Returns 0 on success. Otherwise, 1.
 */
static uint32_t attest_get_tlv_data(uint8_t   minor_type,
                                    uint8_t  *token_buf,
                                    uint8_t **tlv_data_ptr)
{
    struct shared_data_tlv_header *tlv_header;
    struct shared_data_tlv_entry  *tlv_entry;
    uintptr_t tlv_end;
    uintptr_t tlv_curr;

    tlv_header = (struct shared_data_tlv_header *)token_buf;
    if (tlv_header->tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return 1u;
    }

    /* Get the boundaries of TLV section */
    tlv_end  = (uintptr_t)token_buf + tlv_header->tlv_tot_len;
    tlv_curr = (uintptr_t)token_buf + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section and copy TLVs with requested minor
     * type to the provided buffer.
     */
    for(; tlv_curr < tlv_end; tlv_curr += tlv_entry->tlv_len) {
        tlv_entry = (struct shared_data_tlv_entry *)tlv_curr;
        if (tlv_entry->tlv_minor_type == minor_type) {
            *tlv_data_ptr = (uint8_t *)tlv_entry +
                            SHARED_DATA_ENTRY_HEADER_SIZE;
            return 0;
        }
    }

    return 1u;
}

/*!
 * \brief Static function to generate a random byte stream. It is used as a
 *        challenge object to test attestation service.
 *
 * \param[in]  bytes The size of the random bytes to generate
 * \param[out] buf   Pointer to the buffer to store random bytes
 *
 */
static void generate_challenge(uint32_t bytes, uint8_t *buf)
{
    uint32_t random_value;
    uint8_t  *p_buf = buf;

    while (bytes) {
        /* FixMe: use a valid source to generate random values */
        random_value = rand();

        if (bytes >= sizeof(random_value)) {
            tfm_memcpy(p_buf, &random_value, sizeof(random_value));
            bytes -= sizeof(random_value);
            p_buf += sizeof(random_value);
        } else {
            tfm_memcpy(p_buf, &random_value, bytes);
            bytes = 0;
        }
    }
}

void
register_testsuite_ns_attestation_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(attestation_interface_tests) /
                 sizeof(attestation_interface_tests[0]));

    set_testsuite("Initial Attestation Service secure interface tests"
                  "(TFM_ATTEST_TEST_1XXX)",
                  attestation_interface_tests, list_size, p_test_suite);
}

/*!
 * \brief Positive tests for initial attestation service
 *
 *      - Calling Initial Attestation service to get token
 *      - Check the existence of the fixed set of claims:
 *          - Boot status
 *          - Boot seed
 *          - Instance ID
 *          - Challenge object
 *          - Caller ID
 *          - Implementation ID
 *          - Hardware version
 *          - Security lifecycle
 *      - Comparing value of claims:
 *          - Boot seed: Compare boot seed from subsequent calls
 *          - Device ID: Currently compare with fix value
 *          - Challenge: Compare with input object
 *          - Caller ID: Check whether is it in non-secure range
 */
static void tfm_attest_test_1001(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size;
    uint8_t boot_seed_buffer[BOOT_SEED_SIZE];
    uint8_t *tlv_data_ptr;
    int32_t caller_id;
    uint32_t res;

    generate_challenge(TEST_CHALLENGE_OBJ_SIZE, challenge_buffer);

    /* Get attestation token size */
    err = psa_initial_attest_get_token_size(TEST_CHALLENGE_OBJ_SIZE,
                                            &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token size failed");
        return;
    }

    if (token_size != PSA_INITIAL_ATTEST_TOKEN_SIZE) {
        TEST_FAIL("Token size is faulty");
        return;
    }

    /* Get attestation token */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token failed");
        return;
    }

    /* Check IMAGE HASH
     * The image hash cannot be retrieved from image manifest if code runs in
     * NS context, because the image header resides in S memory area.
     */
    res = attest_get_tlv_data(TLV_MINOR_IAS_S_NS_SHA256, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_S_NS_SHA256");
        return;
    }

    /* FixMe: Remove this #if when MPU will be configured properly. Currently
     *        in case of TFM_LVL == 3 unaligned access triggers a usage fault
     *        exception.
     */
#if !defined(TFM_LVL) || (TFM_LVL == 1)
    /* Check INSTANCE_ID */
    res = attest_get_tlv_data(TLV_MINOR_IAS_INSTANCE_ID, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_INSTANCE_ID");
        return;
    }

    /* Check IMPLEMENTATION_ID */
    res = attest_get_tlv_data(TLV_MINOR_IAS_IMPLEMENTATION_ID, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_IMPLEMENTATION_ID");
        return;
    }

    /* Check HARDWARE_VERSION, optional claim */
    res = attest_get_tlv_data(TLV_MINOR_IAS_HW_VERSION, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_HW_VERSION");
        return;
    }
#endif

    /* Check SECURITY LIFECYCLE */
    res = attest_get_tlv_data(TLV_MINOR_IAS_SECURITY_LIFECYCLE, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_SECURITY_LIFECYCLE");
        return;
    }

    /* Check CHALLENGE */
    res = attest_get_tlv_data(TLV_MINOR_IAS_CHALLENGE, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_CHALLENGE");
        return;
    }
    if (tfm_memcmp(tlv_data_ptr, challenge_buffer,
                   TEST_CHALLENGE_OBJ_SIZE) != 0) {
        TEST_FAIL("Faulty claim: TLV_MINOR_IAS_CHALLENGE");
        return;
    }

    /* Check CALLER ID */
    res = attest_get_tlv_data(TLV_MINOR_IAS_CALLER_ID, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_CALLER_ID");
        return;
    }

    caller_id = *((int32_t *)tlv_data_ptr);
    if (!TFM_CLIENT_ID_IS_NS(caller_id)) {
        TEST_FAIL("Faulty claim: TLV_MINOR_IAS_CALLER_ID");
        return;
    }

    /* Check BOOT_SEED */
    res = attest_get_tlv_data(TLV_MINOR_IAS_BOOT_SEED, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_BOOT_SEED");
        return;
    }

    /* Store received boot seed. */
    tfm_memcpy(boot_seed_buffer, tlv_data_ptr, BOOT_SEED_SIZE);

    /* Call attest API again and compare the value of boot seed on subsequent
     * calls. They must be equal within the same boot cycle.
     */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token failed");
        return;
    }

    /* Get boot seed again */
    res = attest_get_tlv_data(TLV_MINOR_IAS_BOOT_SEED, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_BOOT_SEED");
        return;
    }

    /*Compare the values of boot seed on subsequent calls */
    if (tfm_memcmp(tlv_data_ptr, boot_seed_buffer, BOOT_SEED_SIZE) != 0) {
        TEST_FAIL("Faulty claim: TLV_MINOR_IAS_BOOT_SEED");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Negative tests for initial attestation service
 *
 *    - Calling initial attestation service with bigger challenge object than
 *      allowed.
 *    - Calling initial attesttion service with smaller buffer size than the
 *      expected size of the token.
 */
static void tfm_attest_test_1002(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size = TEST_TOKEN_SIZE;

    /* Call with with bigger challenge object than allowed */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TOO_BIG_CHALLENGE_OBJECT,
                                       token_buffer,
                                       &token_size);

    if (err != PSA_ATTEST_ERR_INVALID_INPUT) {
        TEST_FAIL("Attestation should fail with too big challenge object");
        return;
    }

    /* Call with smaller buffer size than size of test token */
    token_size = TOO_SMALL_TOKEN_BUFFER;
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);

    if (err == PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Attestation should fail with too small token buffer");
        return;
    }

    ret->val = TEST_PASSED;
}
