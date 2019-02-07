/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>

#include "test/framework/test_framework_helpers.h"
#include "psa_crypto.h"

#define BIT_SIZE_TEST_KEY (128)
#define BYTE_SIZE_TEST_KEY (BIT_SIZE_TEST_KEY/8)
#define TEST_KEY_SLOT (1)

/* List of tests */
static void tfm_crypto_test_5001(struct test_result_t *ret);

static struct test_t crypto_tests[] = {
    {&tfm_crypto_test_5001, "TFM_CRYPTO_TEST_5001",
     "Secure Key management interface", {0}},
};

void register_testsuite_s_crypto_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(crypto_tests) / sizeof(crypto_tests[0]));

    set_testsuite("Crypto secure interface tests (TFM_CRYPTO_TEST_5XXX)",
                  crypto_tests, list_size, p_test_suite);
}

/**
 * \brief Secure interface test for Crypto
 *
 * \details The scope of this test is to perform a basic validation of
 *          the secure interface for the Key management module.
 */
static void tfm_crypto_test_5001(struct test_result_t *ret)
{
    psa_status_t status;
    uint32_t i = 0;
    const psa_key_slot_t slot = TEST_KEY_SLOT;
    uint8_t data[] = "THIS IS MY KEY1";
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
