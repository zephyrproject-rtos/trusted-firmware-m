/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
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
static void tfm_crypto_test_6004(struct test_result_t *ret);
static void tfm_crypto_test_6005(struct test_result_t *ret);
static void tfm_crypto_test_6006(struct test_result_t *ret);
static void tfm_crypto_test_6007(struct test_result_t *ret);
static void tfm_crypto_test_6008(struct test_result_t *ret);
static void tfm_crypto_test_6009(struct test_result_t *ret);
static void tfm_crypto_test_6010(struct test_result_t *ret);

static struct test_t crypto_veneers_tests[] = {
    {&tfm_crypto_test_6001, "TFM_CRYPTO_TEST_6001",
     "Non Secure Key management interface", {0} },
    {&tfm_crypto_test_6002, "TFM_CRYPTO_TEST_6002",
     "Non Secure Symmetric encryption (AES-128-CBC) interface", {0} },
    {&tfm_crypto_test_6003, "TFM_CRYPTO_TEST_6003",
     "Non Secure Symmetric encryption (AES-128-CFB) interface", {0} },
    {&tfm_crypto_test_6004, "TFM_CRYPTO_TEST_6004",
     "Non Secure Hash (SHA-1) interface", {0} },
    {&tfm_crypto_test_6005, "TFM_CRYPTO_TEST_6005",
     "Non Secure Hash (SHA-224) interface", {0} },
    {&tfm_crypto_test_6006, "TFM_CRYPTO_TEST_6006",
     "Non Secure Hash (SHA-256) interface", {0} },
    {&tfm_crypto_test_6007, "TFM_CRYPTO_TEST_6007",
     "Non Secure Hash (SHA-384) interface", {0} },
    {&tfm_crypto_test_6008, "TFM_CRYPTO_TEST_6008",
     "Non Secure Hash (SHA-512) interface", {0} },
    {&tfm_crypto_test_6009, "TFM_CRYPTO_TEST_6009",
     "Non Secure Hash (MD-5) interface", {0} },
    {&tfm_crypto_test_6010, "TFM_CRYPTO_TEST_6010",
     "Non Secure Hash (RIPEMD-160) interface", {0} },
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
    status = psa_cipher_encrypt_setup(&handle, slot, alg);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting up cipher operation object");
        return;
    }

    /* Set the IV */
    status = psa_cipher_set_iv(&handle, iv, iv_length);
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
        status = psa_cipher_encrypt_setup(&handle_dec, slot, alg);
    } else {
        status = psa_cipher_decrypt_setup(&handle_dec, slot, alg);
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
        status = psa_cipher_set_iv(&handle_dec, iv, iv_length);
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

/*
 * \brief This is the list of algorithms supported by the current
 *        configuration of the crypto engine used by the crypto
 *        service. In case the crypto engine default capabilities
 *        is changed, this list needs to be updated accordingly
 */
static const psa_algorithm_t hash_alg[] = {
    PSA_ALG_SHA_1,
    PSA_ALG_SHA_224,
    PSA_ALG_SHA_256,
    PSA_ALG_SHA_384,
    PSA_ALG_SHA_512,
    PSA_ALG_MD5,
    PSA_ALG_RIPEMD160
};

static const uint8_t hash_val[][PSA_HASH_SIZE(PSA_ALG_SHA_512)] = {
    {0x56, 0x4A, 0x0E, 0x35, 0xF1, 0xC7, 0xBC, 0xD0, /*!< SHA-1 */
     0x7D, 0xCF, 0xB1, 0xBC, 0xC9, 0x16, 0xFA, 0x2E,
     0xF5, 0xBE, 0x96, 0xB2},
    {0x00, 0xD2, 0x90, 0xE2, 0x0E, 0x4E, 0xC1, 0x7E, /*!< SHA-224 */
     0x7A, 0x95, 0xF5, 0x10, 0x5C, 0x76, 0x74, 0x04,
     0x6E, 0xB5, 0x56, 0x5E, 0xE5, 0xE7, 0xBA, 0x15,
     0x6C, 0x23, 0x47, 0xF3},
    {0x6B, 0x22, 0x09, 0x2A, 0x37, 0x1E, 0xF5, 0x14, /*!< SHA-256 */
     0xF7, 0x39, 0x4D, 0xCF, 0xAD, 0x4D, 0x17, 0x46,
     0x66, 0xCB, 0x33, 0xA0, 0x39, 0xD8, 0x41, 0x4E,
     0xF1, 0x2A, 0xD3, 0x4D, 0x69, 0xC3, 0xB5, 0x3E},
    {0x64, 0x79, 0x11, 0xBB, 0x47, 0x4E, 0x47, 0x59, /*!< SHA-384 */
     0x3E, 0x4D, 0xBC, 0x60, 0xA5, 0xF9, 0xBF, 0x9C,
     0xC0, 0xBA, 0x55, 0x0F, 0x93, 0xCA, 0x72, 0xDF,
     0x57, 0x1E, 0x50, 0x56, 0xF9, 0x4A, 0x01, 0xD6,
     0xA5, 0x6F, 0xF7, 0x62, 0x34, 0x4F, 0x48, 0xFD,
     0x9D, 0x15, 0x07, 0x42, 0xB7, 0x72, 0x94, 0xB8},
    {0xB4, 0x1C, 0xA3, 0x6C, 0xA9, 0x67, 0x1D, 0xAD, /*!< SHA-512 */
     0x34, 0x1F, 0xBE, 0x1B, 0x83, 0xC4, 0x40, 0x2A,
     0x47, 0x42, 0x79, 0xBB, 0x21, 0xCA, 0xF0, 0x60,
     0xE4, 0xD2, 0x6E, 0x9B, 0x70, 0x12, 0x34, 0x3F,
     0x55, 0x2C, 0x09, 0x31, 0x0A, 0x5B, 0x40, 0x21,
     0x01, 0xA8, 0x3B, 0x58, 0xE7, 0x48, 0x13, 0x1A,
     0x7E, 0xCD, 0xE1, 0xD2, 0x46, 0x10, 0x58, 0x34,
     0x49, 0x14, 0x4B, 0xAA, 0x89, 0xA9, 0xF5, 0xB1},
    {0x63, 0xFC, 0x11, 0x88, 0xB7, 0x03, 0xDD, 0xD5, /*!< MD-5 */
     0x36, 0xB9, 0x2F, 0xD6, 0x9E, 0x91, 0x96, 0xF8},
    {0xF5, 0x8E, 0xB3, 0xCB, 0xE5, 0xF0, 0x3B, 0xC5, /*!< RIPEMD-160 */
     0x7C, 0x45, 0xE2, 0x49, 0xAA, 0x66, 0xC6, 0x5A,
     0x47, 0xEA, 0x34, 0x91},
    {0x7E, 0x28, 0x13, 0xAE, 0x98, 0xBD, 0x38, 0x6C, /*!< MD-2 */
     0xDC, 0x8C, 0xF8, 0x04, 0xC6, 0x58, 0xA9, 0x69},
    {0xA0, 0xB9, 0x82, 0x4E, 0xE0, 0x74, 0x4F, 0x1E, /*!< MD-4 */
     0xA4, 0x7F, 0xA3, 0xDF, 0xD0, 0x0D, 0x97, 0xEB},
};

static void psa_hash_test(const psa_algorithm_t alg,
                          struct test_result_t *ret)
{
    const char *msg[] = {"This is my test message, ",
                         "please generate a hash for this."};

    const size_t msg_size[] = {25, 32}; /* Length in bytes of msg[0], msg[1] */
    const uint32_t msg_num = sizeof(msg)/sizeof(msg[0]);
    uint32_t idx;

    psa_status_t status;
    psa_hash_operation_t handle;

    /* Setup the hash object for the desired hash*/
    status = psa_hash_setup(&handle, alg);

    if (status != PSA_SUCCESS) {
        if (status == PSA_ERROR_NOT_SUPPORTED) {
            TEST_FAIL("Algorithm NOT SUPPORTED by the implementation");
            return;
        }

        TEST_FAIL("Error setting up hash operation object");
        return;
    }

    /* Update object with all the chunks of message */
    for (idx=0; idx<msg_num; idx++) {
        status = psa_hash_update(&handle,
                                 (const uint8_t *)msg[idx],msg_size[idx]);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error updating the hash operation object");
            return;
        }
    }

    /* Cycle until idx points to the correct index in the algorithm table */
    for (idx=0; hash_alg[idx] != alg; idx++);

    /* Finalise and verify that the hash is as expected */
    status = psa_hash_verify(&handle, &(hash_val[idx][0]), PSA_HASH_SIZE(alg));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error verifying the hash operation object");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_crypto_test_6004(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_1, ret);
}

static void tfm_crypto_test_6005(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_224, ret);
}

static void tfm_crypto_test_6006(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_256, ret);
}

static void tfm_crypto_test_6007(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_384, ret);
}

static void tfm_crypto_test_6008(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_512, ret);
}

static void tfm_crypto_test_6009(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_MD5, ret);
}

static void tfm_crypto_test_6010(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_RIPEMD160, ret);
}
