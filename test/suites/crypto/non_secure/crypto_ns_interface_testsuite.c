/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/test_framework_helpers.h"
#include "tfm_api.h"
#include "psa_crypto.h"

#define BIT_SIZE_TEST_KEY (128)
#define BYTE_SIZE_TEST_KEY (BIT_SIZE_TEST_KEY/8)
#define BYTE_SIZE_CHUNK (16)
#define ENC_DEC_BUFFER_SIZE (32)
#define CIPHER_CFB_DECRYPT_LENGTH (32)
#define CIPHER_CBC_DECRYPT_LENGTH (16)
#define CIPHER_CFB_FLUSH_LENGTH (0)
#define CIPHER_CBC_FLUSH_LENGTH (16)

/* List of tests */
static void tfm_crypto_test_6001(struct test_result_t *ret);
static void tfm_crypto_test_6002(struct test_result_t *ret);
static void tfm_crypto_test_6003(struct test_result_t *ret);

static struct test_t crypto_veneers_tests[] = {
    {&tfm_crypto_test_6001, "TFM_CRYPTO_TEST_6001",
     "Non Secure Key management interface", {0} },
    {&tfm_crypto_test_6002, "TFM_CRYPTO_TEST_6002",
     "Non Secure Symmetric encryption (AES-128-CBC) interface", {0} },
    {&tfm_crypto_test_6003, "TFM_CRYPTO_TEST_6003",
     "Non Secure Symmetric encryption (AES-128-CFB) interface", {0} },
};

void register_testsuite_ns_crypto_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(crypto_veneers_tests) /
                 sizeof(crypto_veneers_tests[0]));

    set_testsuite("Crypto non-secure interface test (TFM_CRYPTO_TEST_6XXX)",
                  crypto_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Non-secure interface test for Crypto
 *
 * \details The scope of this set of tests is to functionally verify
 *          the interfaces specified by psa_crypto.h are working
 *          as expected. This is not meant to cover all possible
 *          scenarios and corner cases.
 *
 */
static void tfm_crypto_test_6001(struct test_result_t *ret)
{
    psa_status_t status = PSA_SUCCESS;
    uint32_t i = 0;
    const psa_key_slot_t slot = 0;
    const uint8_t data[] = "THIS IS MY KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    uint8_t exported_data[sizeof(data)] = {0};
    size_t exported_data_size = 0;

    status = psa_import_key(slot, PSA_KEY_TYPE_AES, data, sizeof(data));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error getting key metadata");
        return;
    }

    if (bits != BIT_SIZE_TEST_KEY) {
        TEST_FAIL("The number of key bits is different from expected");
        return;
    }

    if (type != PSA_KEY_TYPE_AES) {
        TEST_FAIL("The type of the key is different from expected");
        return;
    }

    status = psa_export_key(slot,
                            exported_data,
                            sizeof(data),
                            &exported_data_size);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error exporting a key");
        return;
    }

    if (exported_data_size != BYTE_SIZE_TEST_KEY) {
        TEST_FAIL("Number of bytes of exported key different from expected");
        return;
    }

    /* Check that the exported key is the same as the imported one */
    for (i=0; i<exported_data_size; i++) {
        if (exported_data[i] != data[i]) {
            TEST_FAIL("Exported key doesn't match the imported key");
            return;
        }
    }

    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying the key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_ERROR_EMPTY_SLOT) {
        TEST_FAIL("Key slot should be empty now");
        return;
    }

    ret->val = TEST_PASSED;
}

static uint32_t compare_buffers(const uint8_t *p1,
                                const uint8_t *p2,
                                uint32_t s1,
                                uint32_t s2)
{
    uint32_t i = 0, comp_size = (s1 < s2) ? s1 : s2;

    for (i=0; i<comp_size; i++) {
        if (p1[i] != p2[i]) {
            return 1;
        }
    }

    return 0;
}

static void psa_cipher_test(const psa_algorithm_t alg,
                            struct test_result_t *ret)
{
    psa_cipher_operation_t handle, handle_dec;
    psa_status_t status = PSA_SUCCESS;
    const psa_key_slot_t slot = 0;
    const uint8_t data[] = "THIS IS MY KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    const size_t iv_length = 16;
    const uint8_t iv[] = "012345678901234";
    const uint8_t plain_text[BYTE_SIZE_CHUNK] = "Sixteen bytes!!";
    uint8_t decrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    size_t output_length = 0, expected_output_length = 0;
    uint8_t encrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    uint32_t comp_result;

    /* Import a key on slot 0 */
    status = psa_import_key(slot, PSA_KEY_TYPE_AES, data, sizeof(data));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error getting key metadata");
        return;
    }

    if (bits != BIT_SIZE_TEST_KEY) {
        TEST_FAIL("The number of key bits is different from expected");
        return;
    }

    if (type != PSA_KEY_TYPE_AES) {
        TEST_FAIL("The type of the key is different from expected");
        return;
    }

    /* Setup the encryption object */
    status = psa_encrypt_setup(&handle, slot, alg);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting up cipher operation object");
        return;
    }

    /* Set the IV */
    status = psa_encrypt_set_iv(&handle, iv, iv_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting the IV on the cypher operation object");
        return;
    }

    /* Encrypt one chunk of information */
    status = psa_cipher_update(&handle, plain_text, BYTE_SIZE_CHUNK,
                               encrypted_data, ENC_DEC_BUFFER_SIZE,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error encrypting one chunk of information");
        return;
    }

    if (output_length != BYTE_SIZE_CHUNK) {
        TEST_FAIL("Expected encrypted data length is different from expected");
        return;
    }

    /* Finalise the cipher operation */
    status = psa_cipher_finish(&handle, &encrypted_data[output_length],
                               ENC_DEC_BUFFER_SIZE - output_length,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error finalising the cipher operation");
        return;
    }

    if (output_length != 0) {
        TEST_FAIL("Unexpected output length after finalisation");
        return;
    }

    /* Setup the decryption object */
    if (alg == PSA_ALG_CFB_BASE) {
        /* In CFB mode the object is always in encryption mode */
        status = psa_encrypt_setup(&handle_dec, slot, alg);
    } else {
        status = psa_decrypt_setup(&handle_dec, slot, alg);
    }

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting up cipher operation object");
        return;
    }

    /* Set the IV for decryption */
    if (alg == PSA_ALG_CFB_BASE) {
        /*  In CFB mode the object is in encryption mode, so follow the
         *  encryption flow.
         */
        status = psa_encrypt_set_iv(&handle_dec, iv, iv_length);
    } else {
        status = psa_cipher_update(&handle_dec, iv, iv_length,
                                   encrypted_data, ENC_DEC_BUFFER_SIZE,
                                   &output_length);
    }

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting the IV for decryption");
        return;
    }

    if (alg != PSA_ALG_CFB_BASE) {
        if (output_length != 0) {
            TEST_FAIL("Expected output length is different from expected");
            return;
        }
    }

    /* Decrypt */
    status = psa_cipher_update(&handle_dec, encrypted_data, ENC_DEC_BUFFER_SIZE,
                               decrypted_data, BYTE_SIZE_CHUNK, &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error during decryption");
        return;
    }

    if (alg == PSA_ALG_CFB_BASE) {
        expected_output_length = CIPHER_CFB_DECRYPT_LENGTH;
    } else {
        expected_output_length = CIPHER_CBC_DECRYPT_LENGTH;
    }

    if (output_length != expected_output_length) {
        TEST_FAIL("Decrypted length is different from expected one");
        return;
    }

    /* Check that the plain text matches the decrypted data */
    comp_result = compare_buffers(plain_text, decrypted_data,
                                  sizeof(plain_text), sizeof(decrypted_data));
    if (comp_result != 0) {
        TEST_FAIL("Decrypted data doesn't match with plain text");
        return;
    }

    /* Finalise the cipher operation for decryption (destroys decrypted data) */
    status = psa_cipher_finish(&handle_dec, decrypted_data, BYTE_SIZE_CHUNK,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error finalising the cipher operation");
        return;
    }

    if (alg == PSA_ALG_CFB_BASE) {
        expected_output_length = CIPHER_CFB_FLUSH_LENGTH;
    } else {
        expected_output_length = CIPHER_CBC_FLUSH_LENGTH;
    }

    if (output_length != expected_output_length) {
        TEST_FAIL("After finalising, unexpected decrypted length");
        return;
    }

    /* Destroy the key on slot 0 */
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying a key");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_crypto_test_6002(struct test_result_t *ret)
{
    psa_cipher_test(PSA_ALG_CBC_BASE, ret);
}

static void tfm_crypto_test_6003(struct test_result_t *ret)
{
    psa_cipher_test(PSA_ALG_CFB_BASE, ret);
}
