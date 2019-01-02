/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_s_tests.h"
#include "psa_initial_attestation_api.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "bl2/include/tfm_boot_status.h"
#include "bl2/ext/mcuboot/bootutil/include/bootutil/image.h"
#include "secure_utilities.h"
#include "platform/include/tfm_plat_device_id.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "flash_layout.h"
#include "spm_partition_defs.h"
#include <string.h>
#include <stdlib.h>

static uint8_t token_buffer[TEST_TOKEN_SIZE];
static const uint8_t challenge_buffer[TEST_CHALLENGE_OBJ_SIZE] =
                                     {CHALLENGE_FOR_TEST};

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
 * \brief Get image hash from the image manifest.
 *
 * This function iterates over the image manifest section to look for
 * the image's hash value. This manifest section is also TLV encoded.
 * This function can be called if TFM_LVL == 1, to avoid MemMange fault, which
 * is triggered by enabled MPU at level 3 isolation.
 *
 * Note 1: The access of manifest section must be done with NS alias to avoid
 *         MPC fault, because manifest section is configured to be NS memory.
 *
 * Note 2: This function assumes that active image resides in slot 0 or in RAM
 *         pointing by LOAD_ADDRESS macro. But these assumptions might not be
 *         valid if MCUBOOT_NO_SWAP mode is used.
 *
 * \param[out] Pointer to the location of image hash value in the image
 *             manifest section.
 *
 * \return Returns 0 on success. Otherwise, 1.
 */
#if !defined(TFM_LVL) || (TFM_LVL == 1)
/* FixMe: This offset should be calculated per device */
#define USUAL_S_NS_ALIAS_OFFSET 0x10000000u
static uint32_t attest_get_s_ns_sha256(uint8_t **hash)
{
    struct image_header *image_header;
    struct image_tlv_info *tlv_info;
    struct image_tlv *tlv_entry;
    uint32_t image_header_address;
    uint8_t *off;
    uint8_t *end;

#ifdef LOAD_ADDRESS
    image_header_address = LOAD_ADDRESS;
#else
    image_header_address = (FLASH_BASE_ADDRESS + FLASH_AREA_IMAGE_0_OFFSET);
#endif
    image_header = (struct image_header *)image_header_address;
    /* Calculates NS memory alias of manifest section */
    tlv_info     = (struct image_tlv_info *)(image_header_address +
                                             image_header->ih_hdr_size +
                                             image_header->ih_img_size -
                                             USUAL_S_NS_ALIAS_OFFSET);

    end = (uint8_t *)tlv_info + tlv_info->it_tlv_tot;
    off = (uint8_t *)tlv_info + sizeof(struct image_tlv_info);

    for (; off < end; off += sizeof(struct image_tlv) + tlv_entry->it_len) {
        tlv_entry = (struct image_tlv *)(off);
        if (tlv_entry->it_type == IMAGE_TLV_SHA256) {
                *hash = (uint8_t *)tlv_entry + sizeof(struct image_tlv);
                return 0;
        }
    }

    return 1u;
}
#endif

void
register_testsuite_s_attestation_interface(struct test_suite_t *p_test_suite)
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
 *      - Comparing value of claims:
 *          - Boot status: Get boot status from token and from image manifest
 *                         and compare them
 *          - Boot seed: Compare boot seed from subsequent calls
 *          - Device ID: Currently compare with fix value
 *          - Challenge: Compare with input object
 *          - Caller ID: Check whether is it in non-secure range
 */
static void tfm_attest_test_1001(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size = TEST_TOKEN_SIZE;
    uint8_t boot_seed_buffer[BOOT_SEED_SIZE];
    uint8_t *tlv_data_ptr;
    int32_t caller_id;
    uint32_t res;

    /* Get attestation token
     * FixMe: Hard coded challenge is used, because currently there is no
     *        support for random source(RNG, rand(), etc.) on secure side.
     */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token failed");
        return;
    }

    /* Check IMAGE HASH */
    res = attest_get_tlv_data(TLV_MINOR_IAS_S_NS_SHA256, token_buffer,
                              &tlv_data_ptr);
    if (res != 0) {
        TEST_FAIL("Missing claim: TLV_MINOR_IAS_S_NS_SHA256");
        return;
    }
    /* Extract image hash from manifest data and compare with claim.
     * This can be done only from secure side if S_MPU is not enabled.
     */
#if !defined(TFM_LVL) || (TFM_LVL == 1)
    uint8_t *hash_from_image;
    res = attest_get_s_ns_sha256(&hash_from_image);
    if (res == 0) {
        if (tfm_memcmp(tlv_data_ptr, hash_from_image, 32) != 0) {
            TEST_FAIL("Faulty claim: TLV_MINOR_IAS_S_NS_SHA256");
            return;
        }
    }
#endif

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
    if (caller_id < TFM_SP_BASE) {
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
 *    - Calling initial attestation service with smaller buffer size than the
 *      expected size of the token.
 */
static void tfm_attest_test_1002(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size = TEST_TOKEN_SIZE;

    /* Call with smaller buffer size than size of test token */
    token_size = TOO_SMALL_TOKEN_BUFFER;
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);

    if (err != PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW) {
        TEST_FAIL("Attestation should fail with small buffer");
        return;
    }

    ret->val = TEST_PASSED;
}
