/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "test/framework/test_framework_helpers.h"
#include "tfm_api.h"
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"

#define BIT_SIZE_TEST_KEY (128)
#define BIT_SIZE_TEST_LONG_KEY (256)
#define BYTE_SIZE_TEST_KEY (BIT_SIZE_TEST_KEY/8)
#define BYTE_SIZE_CHUNK (16)
#define ENC_DEC_BUFFER_SIZE (32)
#define ASSOCIATED_DATA_SIZE (24)
#define TEST_KEY_SLOT (1)
#define TEST_MAX_KEY_LENGTH (64)

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
static void tfm_crypto_test_6011(struct test_result_t *ret);
static void tfm_crypto_test_6012(struct test_result_t *ret);
static void tfm_crypto_test_6013(struct test_result_t *ret);
static void tfm_crypto_test_6014(struct test_result_t *ret);
static void tfm_crypto_test_6015(struct test_result_t *ret);
static void tfm_crypto_test_6016(struct test_result_t *ret);
static void tfm_crypto_test_6017(struct test_result_t *ret);
static void tfm_crypto_test_6018(struct test_result_t *ret);
static void tfm_crypto_test_6019(struct test_result_t *ret);
static void tfm_crypto_test_6020(struct test_result_t *ret);
static void tfm_crypto_test_6021(struct test_result_t *ret);
static void tfm_crypto_test_6022(struct test_result_t *ret);
static void tfm_crypto_test_6023(struct test_result_t *ret);
static void tfm_crypto_test_6024(struct test_result_t *ret);
static void tfm_crypto_test_6030(struct test_result_t *ret);
static void tfm_crypto_test_6031(struct test_result_t *ret);
static void tfm_crypto_test_6032(struct test_result_t *ret);
static void tfm_crypto_test_6033(struct test_result_t *ret);

static struct test_t crypto_veneers_tests[] = {
    {&tfm_crypto_test_6001, "TFM_CRYPTO_TEST_6001",
     "Non Secure Key management interface", {0} },
    {&tfm_crypto_test_6002, "TFM_CRYPTO_TEST_6002",
     "Non Secure Symmetric encryption (AES-128-CBC) interface", {0} },
    {&tfm_crypto_test_6003, "TFM_CRYPTO_TEST_6003",
     "Non Secure Symmetric encryption (AES-128-CFB) interface", {0} },
    {&tfm_crypto_test_6004, "TFM_CRYPTO_TEST_6004",
     "Non Secure Symmetric encryption (DES-128-CBC) interface", {0} },
    {&tfm_crypto_test_6005, "TFM_CRYPTO_TEST_6005",
     "Non Secure Symmetric encryption (CAMELLIA-128-CTR) interface", {0} },
    {&tfm_crypto_test_6006, "TFM_CRYPTO_TEST_6006",
     "Non Secure Symmetric encryption invalid cipher (DES-128-RC4)", {0} },
    {&tfm_crypto_test_6007, "TFM_CRYPTO_TEST_6007",
     "Non Secure Symmetric encryption invalid cipher (AES-128-GCM)", {0} },
    {&tfm_crypto_test_6008, "TFM_CRYPTO_TEST_6008",
     "Non Secure Symmetric encryption invalid cipher (DES-152-CBC)", {0} },
    {&tfm_crypto_test_6009, "TFM_CRYPTO_TEST_6009",
     "Non Secure Symmetric encryption invalid cipher (HMAC-128-CFB)", {0} },
    {&tfm_crypto_test_6010, "TFM_CRYPTO_TEST_6010",
     "Non Secure Hash (SHA-1) interface", {0} },
    {&tfm_crypto_test_6011, "TFM_CRYPTO_TEST_6011",
     "Non Secure Hash (SHA-224) interface", {0} },
    {&tfm_crypto_test_6012, "TFM_CRYPTO_TEST_6012",
     "Non Secure Hash (SHA-256) interface", {0} },
    {&tfm_crypto_test_6013, "TFM_CRYPTO_TEST_6013",
     "Non Secure Hash (SHA-384) interface", {0} },
    {&tfm_crypto_test_6014, "TFM_CRYPTO_TEST_6014",
     "Non Secure Hash (SHA-512) interface", {0} },
    {&tfm_crypto_test_6015, "TFM_CRYPTO_TEST_6015",
     "Non Secure Hash (MD-5) interface", {0} },
    {&tfm_crypto_test_6016, "TFM_CRYPTO_TEST_6016",
     "Non Secure Hash (RIPEMD-160) interface", {0} },
    {&tfm_crypto_test_6017, "TFM_CRYPTO_TEST_6017",
     "Non Secure Hash (MD-2) interface", {0} },
    {&tfm_crypto_test_6018, "TFM_CRYPTO_TEST_6018",
     "Non Secure Hash (MD-4) interface", {0} },
    {&tfm_crypto_test_6019, "TFM_CRYPTO_TEST_6019",
     "Non Secure HMAC (SHA-1) interface", {0} },
    {&tfm_crypto_test_6020, "TFM_CRYPTO_TEST_6020",
     "Non Secure HMAC (SHA-256) interface", {0} },
    {&tfm_crypto_test_6021, "TFM_CRYPTO_TEST_6021",
     "Non Secure HMAC (SHA-384) interface", {0} },
    {&tfm_crypto_test_6022, "TFM_CRYPTO_TEST_6022",
     "Non Secure HMAC (SHA-512) interface", {0} },
    {&tfm_crypto_test_6023, "TFM_CRYPTO_TEST_6023",
     "Non Secure HMAC (MD-5) interface", {0} },
    {&tfm_crypto_test_6024, "TFM_CRYPTO_TEST_6024",
     "Non Secure HMAC with long key (SHA-1) interface", {0} },
    {&tfm_crypto_test_6030, "TFM_CRYPTO_TEST_6030",
     "Non Secure AEAD (AES-128-CCM) interface", {0} },
    {&tfm_crypto_test_6031, "TFM_CRYPTO_TEST_6031",
     "Non Secure AEAD (AES-128-GCM) interface", {0} },
    {&tfm_crypto_test_6032, "TFM_CRYPTO_TEST_6032",
     "Non Secure key policy interface", {0} },
    {&tfm_crypto_test_6033, "TFM_CRYPTO_TEST_6033",
     "Non Secure key policy check permissions", {0} },
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
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    const uint8_t data[] = "THIS IS MY KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    uint8_t exported_data[sizeof(data)] = {0};
    size_t exported_data_size = 0;
    psa_key_policy_t policy;

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, PSA_KEY_USAGE_EXPORT, 0);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

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

static void psa_cipher_test(const psa_key_type_t key_type,
                            const psa_algorithm_t alg,
                            struct test_result_t *ret)
{
    psa_cipher_operation_t handle, handle_dec;
    psa_status_t status = PSA_SUCCESS;
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    const uint8_t data[] = "THIS IS MY KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    const size_t iv_length = PSA_BLOCK_CIPHER_BLOCK_SIZE(key_type);
    const uint8_t iv[] = "012345678901234";
    const uint8_t plain_text[BYTE_SIZE_CHUNK] = "Sixteen bytes!!";
    uint8_t decrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    size_t output_length = 0, total_output_length = 0;
    uint8_t encrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    uint32_t comp_result;
    psa_key_policy_t policy;
    psa_key_usage_t usage = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    uint32_t i;

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, usage, alg);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    ret->val = TEST_PASSED;

    /* Import a key on slot 0 */
    status = psa_import_key(slot, key_type, data, sizeof(data));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error getting key metadata");
        goto destroy_key;
    }

    if (bits != BIT_SIZE_TEST_KEY) {
        TEST_FAIL("The number of key bits is different from expected");
        goto destroy_key;
    }

    if (type != key_type) {
        TEST_FAIL("The type of the key is different from expected");
        goto destroy_key;
    }

    /* Setup the encryption object */
    status = psa_cipher_encrypt_setup(&handle, slot, alg);
    if (status != PSA_SUCCESS) {
        if (status == PSA_ERROR_NOT_SUPPORTED) {
            TEST_FAIL("Algorithm NOT SUPPORTED by the implementation");
        } else {
            TEST_FAIL("Error setting up cipher operation object");
        }
        goto destroy_key;
    }

    /* Set the IV */
    status = psa_cipher_set_iv(&handle, iv, iv_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting the IV on the cypher operation object");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    /* Encrypt one chunk of information */
    status = psa_cipher_update(&handle, plain_text, BYTE_SIZE_CHUNK,
                               encrypted_data, ENC_DEC_BUFFER_SIZE,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error encrypting one chunk of information");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    if (output_length != BYTE_SIZE_CHUNK) {
        TEST_FAIL("Expected encrypted data length is different from expected");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    /* Finalise the cipher operation */
    status = psa_cipher_finish(&handle, &encrypted_data[output_length],
                               ENC_DEC_BUFFER_SIZE - output_length,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error finalising the cipher operation");
        status = psa_cipher_abort(&handle);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    if (output_length != 0) {
        TEST_FAIL("Unexpected output length after finalisation");
        goto destroy_key;
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
        goto destroy_key;
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
        status = psa_cipher_abort(&handle_dec);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    if (alg != PSA_ALG_CFB_BASE) {
        if (output_length != 0) {
            TEST_FAIL("Expected output length is different from expected");
            status = psa_cipher_abort(&handle_dec);
            if (status != PSA_SUCCESS) {
                TEST_FAIL("Error aborting the operation");
            }
            goto destroy_key;
        }
    }

    /* Decrypt */
    for (i = 0; i < ENC_DEC_BUFFER_SIZE; i += BYTE_SIZE_CHUNK) {
        status = psa_cipher_update(&handle_dec,
                                   (encrypted_data + i), BYTE_SIZE_CHUNK,
                                   (decrypted_data + total_output_length),
                                   (ENC_DEC_BUFFER_SIZE - total_output_length),
                                   &output_length);

        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error during decryption");
            status = psa_cipher_abort(&handle_dec);
            if (status != PSA_SUCCESS) {
                TEST_FAIL("Error aborting the operation");
            }
            goto destroy_key;
        }

        total_output_length += output_length;
    }

    /* Check that the plain text matches the decrypted data */
    comp_result = compare_buffers(plain_text, decrypted_data,
                                  sizeof(plain_text), sizeof(decrypted_data));
    if (comp_result != 0) {
        TEST_FAIL("Decrypted data doesn't match with plain text");
        status = psa_cipher_abort(&handle_dec);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    /* Finalise the cipher operation for decryption (destroys decrypted data) */
    status = psa_cipher_finish(&handle_dec, decrypted_data, BYTE_SIZE_CHUNK,
                               &output_length);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error finalising the cipher operation");
        status = psa_cipher_abort(&handle_dec);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error aborting the operation");
        }
        goto destroy_key;
    }

    total_output_length += output_length;

    /* Check that the decrypted length is equal to the original length */
    if (total_output_length != ENC_DEC_BUFFER_SIZE) {
        TEST_FAIL("After finalising, unexpected decrypted length");
        goto destroy_key;
    }

destroy_key:
    /* Destroy the key on slot 0 */
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying a key");
    }
}

static void tfm_crypto_test_6002(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_CBC_BASE, ret);
}

static void tfm_crypto_test_6003(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_CFB_BASE, ret);
}

static void tfm_crypto_test_6004(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_DES, PSA_ALG_CBC_BASE, ret);
}

static void tfm_crypto_test_6005(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_CAMELLIA, PSA_ALG_CTR, ret);
}

/**
 * \brief Tests invalid key type and algorithm combinations for block ciphers.
 *
 * \param[in]  key_type  PSA key type
 * \param[in]  alg       PSA algorithm
 * \param[in]  key_size  Key size
 * \param[out] ret       Test result
 */
static void psa_test_invalid_cipher(const psa_key_type_t key_type,
                                    const psa_algorithm_t alg,
                                    const size_t key_size,
                                    struct test_result_t *ret)
{
    psa_status_t status;
    psa_cipher_operation_t handle;
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    uint8_t data[TEST_MAX_KEY_LENGTH];
    psa_key_policy_t policy;
    psa_key_usage_t usage = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, usage, alg);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    /* Fill the key data */
    memset(data, 'A', key_size);

    /* Import a key to slot 0 */
    status = psa_import_key(slot, key_type, data, key_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    /* Setup the encryption object */
    status = psa_cipher_encrypt_setup(&handle, slot, alg);
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Should not successfully setup an invalid cipher");
        psa_destroy_key(slot);
        return;
    }

    /* Destroy the key in slot 0 */
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying a key");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_crypto_test_6006(struct test_result_t *ret)
{
    /* Invalid combination: DES keytypes are not usable with a stream cipher */
    psa_test_invalid_cipher(PSA_KEY_TYPE_DES, PSA_ALG_ARC4, 16, ret);
}

static void tfm_crypto_test_6007(struct test_result_t *ret)
{
    /* GCM is an AEAD mode */
    psa_test_invalid_cipher(PSA_KEY_TYPE_AES, PSA_ALG_GCM, 16, ret);
}

static void tfm_crypto_test_6008(struct test_result_t *ret)
{
    psa_status_t status;
    psa_key_policy_t policy;
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    const uint8_t data[19] = {0};

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_CBC_BASE);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    /* DES does not support 152-bit keys */
    status = psa_import_key(slot, PSA_KEY_TYPE_DES, data, sizeof(data));
    if (status != PSA_ERROR_NOT_SUPPORTED) {
        TEST_FAIL("Should not successfully import with an invalid key length");
        return;
    }
}

static void tfm_crypto_test_6009(struct test_result_t *ret)
{
    /* HMAC is not a block cipher */
    psa_test_invalid_cipher(PSA_KEY_TYPE_HMAC, PSA_ALG_CFB_BASE, 16, ret);
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
    PSA_ALG_RIPEMD160,
    PSA_ALG_MD2,
    PSA_ALG_MD4
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

static void tfm_crypto_test_6010(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_1, ret);
}

static void tfm_crypto_test_6011(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_224, ret);
}

static void tfm_crypto_test_6012(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_256, ret);
}

static void tfm_crypto_test_6013(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_384, ret);
}

static void tfm_crypto_test_6014(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_512, ret);
}

static void tfm_crypto_test_6015(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_MD5, ret);
}

static void tfm_crypto_test_6016(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_RIPEMD160, ret);
}

static void tfm_crypto_test_6017(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_MD2, ret);
}

static void tfm_crypto_test_6018(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_MD4, ret);
}

static const uint8_t hmac_val[][PSA_HASH_SIZE(PSA_ALG_SHA_512)] = {
    {0x0d, 0xa6, 0x9d, 0x02, 0x43, 0x17, 0x3e, 0x7e, /*!< SHA-1 */
     0xe7, 0x3b, 0xc6, 0xa9, 0x51, 0x06, 0x8a, 0xea,
     0x12, 0xb0, 0xa7, 0x1d},
    {0xc1, 0x9f, 0x19, 0xac, 0x05, 0x65, 0x5f, 0x02, /*!< SHA-224 */
     0x1b, 0x64, 0x32, 0xd9, 0xb1, 0x49, 0xba, 0x75,
     0x05, 0x60, 0x52, 0x4e, 0x78, 0xfa, 0x61, 0xc9,
     0x37, 0x5d, 0x7f, 0x58},
    {0x94, 0x37, 0xbe, 0xb5, 0x7f, 0x7c, 0x5c, 0xb0, /*!< SHA-256 */
     0x0a, 0x92, 0x4d, 0xd3, 0xba, 0x7e, 0xb1, 0x1a,
     0xdb, 0xa2, 0x25, 0xb2, 0x82, 0x8e, 0xdf, 0xbb,
     0x61, 0xbf, 0x91, 0x1d, 0x28, 0x23, 0x4a, 0x04},
    {0x94, 0x21, 0x9b, 0xc3, 0xd5, 0xed, 0xe6, 0xee,  /*!< SHA-384 */
     0x42, 0x10, 0x5a, 0x58, 0xa4, 0x4d, 0x67, 0x87,
     0x16, 0xa2, 0xa7, 0x6c, 0x2e, 0xc5, 0x85, 0xb7,
     0x6a, 0x4c, 0x90, 0xb2, 0x73, 0xee, 0x58, 0x3c,
     0x59, 0x16, 0x67, 0xf3, 0x6f, 0x30, 0x99, 0x1c,
     0x2a, 0xf7, 0xb1, 0x5f, 0x45, 0x83, 0xf5, 0x9f},
    {0x8f, 0x76, 0xef, 0x12, 0x0b, 0x92, 0xc2, 0x06, /*!< SHA-512 */
     0xce, 0x01, 0x18, 0x75, 0x84, 0x96, 0xd9, 0x6f,
     0x23, 0x88, 0xd4, 0xf8, 0xcf, 0x79, 0xf8, 0xcf,
     0x27, 0x12, 0x9f, 0xa6, 0x7e, 0x87, 0x9a, 0x68,
     0xee, 0xe2, 0xe7, 0x1d, 0x4b, 0xf2, 0x87, 0xc0,
     0x05, 0x6a, 0xbd, 0x7f, 0x9d, 0xff, 0xaa, 0xf3,
     0x9a, 0x1c, 0xb7, 0xb7, 0xbd, 0x03, 0x61, 0xa3,
     0xa9, 0x6a, 0x5d, 0xb2, 0x81, 0xe1, 0x6f, 0x1f},
    {0x26, 0xfb, 0x68, 0xd2, 0x28, 0x17, 0xc2, 0x9c, /*!< MD-5 */
     0xbe, 0xed, 0x95, 0x16, 0x82, 0xb0, 0xd8, 0x99},
    {0x5c, 0xd9, 0x49, 0xc8, 0x66, 0x7a, 0xfa, 0x79, /*!< RIPEMD-160 */
     0xa8, 0x88, 0x2e, 0x53, 0xf4, 0xee, 0xc0, 0x2d,
     0x1e, 0xf0, 0x80, 0x25},
    {0x0c, 0x8c, 0x8c, 0x16, 0x49, 0x92, 0x76, 0xf1, /*!< MD-2 */
     0xc4, 0xcc, 0xdc, 0x9f, 0x7c, 0xb2, 0xeb, 0x87},
    {0x44, 0xdf, 0x1b, 0x97, 0xe9, 0xe8, 0xd3, 0xb0, /*!< MD-4 */
     0xe8, 0x8d, 0xad, 0xdb, 0x86, 0xab, 0xa6, 0xc6},
};

static const uint8_t long_key_hmac_val[PSA_HASH_SIZE(PSA_ALG_SHA_1)] = {
    0xb5, 0x06, 0x7b, 0x9a, 0xb9, 0xe7, 0x47, 0x3c, /*!< SHA-1 */
    0x2d, 0x44, 0x46, 0x1f, 0x4a, 0xbd, 0x22, 0x53,
    0x9c, 0x05, 0x34, 0x34
};

static void psa_mac_test(const psa_algorithm_t alg,
                         uint8_t use_long_key,
                         struct test_result_t *ret)
{
    const char *msg[] = {"This is my test message, ",
                         "please generate a hmac for this."};
    const size_t msg_size[] = {25, 32}; /* Length in bytes of msg[0], msg[1] */
    const uint32_t msg_num = sizeof(msg)/sizeof(msg[0]);
    uint32_t idx;

    const psa_key_slot_t slot = TEST_KEY_SLOT;
    const uint8_t data[] = "THIS IS MY KEY1";
    const uint8_t long_data[] = "THIS IS MY UNCOMMONLY LONG KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    size_t bit_size_test_key = 0;
    psa_status_t status;
    psa_mac_operation_t handle;
    psa_key_policy_t policy;
    psa_key_usage_t usage = PSA_KEY_USAGE_VERIFY;

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, usage, alg);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    ret->val = TEST_PASSED;

    /* Import key on slot 0 */
    if (use_long_key == 1) {
        status = psa_import_key(slot,
                                PSA_KEY_TYPE_HMAC,
                                long_data,
                                sizeof(long_data));
    } else {
        status = psa_import_key(slot, PSA_KEY_TYPE_HMAC, data, sizeof(data));
    }

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error getting key metadata");
        goto destroy_key_mac;
    }

    if (use_long_key == 1) {
        bit_size_test_key = BIT_SIZE_TEST_LONG_KEY;
    } else {
        bit_size_test_key = BIT_SIZE_TEST_KEY;
    }

    if (bits != bit_size_test_key) {
        TEST_FAIL("The number of key bits is different from expected");
        goto destroy_key_mac;
    }

    if (type != PSA_KEY_TYPE_HMAC) {
        TEST_FAIL("The type of the key is different from expected");
        goto destroy_key_mac;
    }

    /* Setup the mac object for hmac */
    status = psa_mac_verify_setup(&handle, slot, alg);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error setting up mac operation object");
        goto destroy_key_mac;
    }

    /* Update object with all the chunks of message */
    for (idx=0; idx<msg_num; idx++) {
        status = psa_mac_update(&handle,
                                (const uint8_t *)msg[idx],
                                msg_size[idx]);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Error during mac operation");
            goto destroy_key_mac;
        }
    }

    /* Cycle until idx points to the correct index in the algorithm table */
    for (idx=0; hash_alg[idx] != PSA_ALG_HMAC_HASH(alg); idx++);

    /* Finalise and verify the mac value */
    if (use_long_key == 1) {
        status = psa_mac_verify_finish(&handle,
                                       &(long_key_hmac_val[0]),
                                       PSA_HASH_SIZE(PSA_ALG_HMAC_HASH(alg)));
    } else {
        status = psa_mac_verify_finish(&handle,
                                       &(hmac_val[idx][0]),
                                       PSA_HASH_SIZE(PSA_ALG_HMAC_HASH(alg)));
    }
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error during finalising the mac operation");
        goto destroy_key_mac;
    }

destroy_key_mac:
    /* Destroy the key */
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying the key");
    }
}

static void tfm_crypto_test_6019(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_1), 0, ret);
}

static void tfm_crypto_test_6020(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_256), 0, ret);
}

static void tfm_crypto_test_6021(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_384), 0, ret);
}

static void tfm_crypto_test_6022(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_512), 0, ret);
}

static void tfm_crypto_test_6023(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_MD5), 0, ret);
}

static void tfm_crypto_test_6024(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_1), 1, ret);
}

static void psa_aead_test(const psa_key_type_t key_type,
                          const psa_algorithm_t alg,
                          struct test_result_t *ret)
{
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    const size_t nonce_length = 12;
    const uint8_t nonce[] = "01234567890";
    const uint8_t plain_text[BYTE_SIZE_CHUNK] = "Sixteen bytes!!";
    const uint8_t associated_data[ASSOCIATED_DATA_SIZE] =
                                                      "This is associated data";
    uint8_t encrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    size_t encrypted_data_length = 0, decrypted_data_length = 0;
    uint8_t decrypted_data[ENC_DEC_BUFFER_SIZE] = {0};
    psa_status_t status;
    const uint8_t data[] = "THIS IS MY KEY1";
    psa_key_type_t type = PSA_KEY_TYPE_NONE;
    size_t bits = 0;
    uint32_t comp_result;
    psa_key_policy_t policy;
    psa_key_usage_t usage = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, usage, alg);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    ret->val = TEST_PASSED;

    /* Import a key on slot 0 */
    status = psa_import_key(slot, key_type, data, sizeof(data));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error importing a key");
        return;
    }

    status = psa_get_key_information(slot, &type, &bits);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error getting key metadata");
        goto destroy_key_aead;
    }

    if (bits != BIT_SIZE_TEST_KEY) {
        TEST_FAIL("The number of key bits is different from expected");
        goto destroy_key_aead;
    }

    if (type != key_type) {
        TEST_FAIL("The type of the key is different from expected");
        goto destroy_key_aead;
    }

    /* Perform AEAD encryption */
    status = psa_aead_encrypt(slot, alg, nonce, nonce_length,
                              associated_data,
                              sizeof(associated_data),
                              plain_text,
                              sizeof(plain_text),
                              encrypted_data,
                              sizeof(encrypted_data),
                              &encrypted_data_length );

    if (status != PSA_SUCCESS) {
        if (status == PSA_ERROR_NOT_SUPPORTED) {
            TEST_FAIL("Algorithm NOT SUPPORTED by the implementation");
            goto destroy_key_aead;
        }

        TEST_FAIL("Error performing AEAD encryption");
        goto destroy_key_aead;
    }

    if (sizeof(plain_text) != (encrypted_data_length-PSA_AEAD_TAG_SIZE(alg))) {
        TEST_FAIL("Encrypted data length is different than expected");
        goto destroy_key_aead;
    }

    /* Perform AEAD decryption */
    status = psa_aead_decrypt(slot, alg, nonce, nonce_length,
                              associated_data,
                              sizeof(associated_data),
                              encrypted_data,
                              encrypted_data_length,
                              decrypted_data,
                              sizeof(decrypted_data),
                              &decrypted_data_length );

    if (status != PSA_SUCCESS) {
        if (status == PSA_ERROR_NOT_SUPPORTED) {
            TEST_FAIL("Algorithm NOT SUPPORTED by the implementation");
        } else {
            TEST_FAIL("Error performing AEAD decryption");
        }

        goto destroy_key_aead;
    }

    if (sizeof(plain_text) != decrypted_data_length) {
        TEST_FAIL("Decrypted data length is different from plain text");
        goto destroy_key_aead;
    }

    /* Check that the decrypted data is the same as the original data */
    comp_result = compare_buffers(plain_text,
                                  decrypted_data,
                                  sizeof(plain_text),
                                  decrypted_data_length);

    if (comp_result != 0) {
        TEST_FAIL("Decrypted data doesn't match with plain text");
        goto destroy_key_aead;
    }


destroy_key_aead:
    /* Destroy the key on slot 0 */
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Error destroying a key");
    }
}

static void tfm_crypto_test_6030(struct test_result_t *ret)
{
    psa_aead_test(PSA_KEY_TYPE_AES, PSA_ALG_CCM, ret);
}

static void tfm_crypto_test_6031(struct test_result_t *ret)
{
    psa_aead_test(PSA_KEY_TYPE_AES, PSA_ALG_GCM, ret);
}

static void tfm_crypto_test_6032(struct test_result_t *ret)
{
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_CBC_BASE;
    psa_algorithm_t alg_out;
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_VOLATILE;
    psa_key_lifetime_t lifetime_out;
    psa_key_policy_t policy;
    psa_key_policy_t policy_out;
    psa_key_slot_t slot = TEST_KEY_SLOT;
    psa_key_usage_t usage = PSA_KEY_USAGE_EXPORT;
    psa_key_usage_t usage_out;

    /* Initialise the key policy */
    psa_key_policy_init(&policy);

    /* Verify that initialised policy forbids all usage */
    usage_out = psa_key_policy_get_usage(&policy);
    if (usage_out != 0) {
        TEST_FAIL("Unexpected usage value");
        return;
    }

    alg_out = psa_key_policy_get_algorithm(&policy);
    if (alg_out != 0) {
        TEST_FAIL("Unexpected algorithm value");
        return;
    }

    /* Set the key policy values */
    psa_key_policy_set_usage(&policy, usage, alg);

    /* Check that the key policy has the correct usage */
    usage_out = psa_key_policy_get_usage(&policy);
    if (usage_out != usage) {
        TEST_FAIL("Unexpected usage value");
        return;
    }

    /* Check that the key policy has the correct algorithm */
    alg_out = psa_key_policy_get_algorithm(&policy);
    if (alg_out != alg) {
        TEST_FAIL("Unexpected algorithm value");
        return;
    }

    /* Set the key policy for the key slot */
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    /* Check the key slot has the correct key policy */
    status = psa_get_key_policy(slot, &policy_out);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to get key policy");
        return;
    }

    usage_out = psa_key_policy_get_usage(&policy_out);
    if (usage_out != usage) {
        TEST_FAIL("Unexpected usage value");
        return;
    }

    alg_out = psa_key_policy_get_algorithm(&policy_out);
    if (alg_out != alg) {
        TEST_FAIL("Unexpected algorithm value");
        return;
    }

    /* Set the key lifetime for the key slot */
    status = psa_set_key_lifetime(slot, lifetime);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key lifetime");
        return;
    }

    /* Check the key slot has the correct key lifetime */
    status = psa_get_key_lifetime(slot, &lifetime_out);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to get key lifetime");
        return;
    }

    if (lifetime_out != lifetime) {
        TEST_FAIL("Unexpected key lifetime value");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_crypto_test_6033(struct test_result_t *ret)
{
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_CBC_BASE;
    psa_cipher_operation_t handle;
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_VOLATILE;
    psa_key_policy_t policy;
    psa_key_slot_t slot = TEST_KEY_SLOT;
    psa_key_usage_t usage = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    size_t data_len;
    const uint8_t data[] = "THIS IS MY KEY1";
    uint8_t data_out[sizeof(data)];

    ret->val = TEST_PASSED;

    /* Setup the key policy */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, usage, alg);
    status = psa_set_key_policy(slot, &policy);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key policy");
        return;
    }

    /* Set the key lifetime for the key slot */
    status = psa_set_key_lifetime(slot, lifetime);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to set key lifetime");
        return;
    }

    /* Import a key to the slot for which policy has been set */
    status = psa_import_key(slot, PSA_KEY_TYPE_AES, data, sizeof(data));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to import a key");
        return;
    }

    /* Setup a cipher permitted by the key policy */
    status = psa_cipher_encrypt_setup(&handle, slot, alg);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to setup cipher operation");
        goto destroy_key;
    }

    status = psa_cipher_abort(&handle);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to abort cipher operation");
        goto destroy_key;
    }

    /* Attempt to setup a cipher with an alg not permitted by the policy */
    status = psa_cipher_encrypt_setup(&handle, slot, PSA_ALG_CFB_BASE);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Was able to setup cipher operation with wrong alg");
        goto destroy_key;
    }

    /* Attempt to export the key, which is forbidden by the key policy */
    status = psa_export_key(slot, data_out, sizeof(data_out), &data_len);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Should not be able to export key without correct usage");
        goto destroy_key;
    }

destroy_key:
    status = psa_destroy_key(slot);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Failed to destroy key");
    }
}
