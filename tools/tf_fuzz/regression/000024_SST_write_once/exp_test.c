/*
 * Test purpose:
 *     to verify we can't write multiple times to a write-once SST asset
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "../sst/non_secure/ns_test_helpers.h"
#include "psa/protected_storage.h"
#include "test/framework/test_framework_helpers.h"
#include "tfm_memory_utils.h"
#include "psa/crypto.h"
#include "psa/crypto_sizes.h"

/* This is not yet right for how to run a test;  need to register tests, etc. */

void test_thread (struct test_result_t *ret) {
    psa_status_t crypto_status;  /* result from Crypto calls */
    psa_status_t sst_status;
    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t writeOnce_set_data\[\] = "@@012@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t writeOnce_set_length = \d+;
    static uint8_t writeOnce_set_data_1\[\] = "@@013@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t writeOnce_set_length_1 = \d+;
    static uint8_t writeOnce_set_data_2\[\] = "@@014@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t writeOnce_set_length_2 = \d+;
    (void)sst_status;
        /* "void" to prevent unused-variable warning, since the variable may not
         * be used in this particular test case.
         */

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to verify we can't write multiple times to a write-once SST asset");


    /* PSA calls to test: */
    /\* Creating SST asset "writeOnce," with data "@@012@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, writeOnce_set_length, writeOnce_set_data,
                            PSA_STORAGE_FLAG_WRITE_ONCE);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Resetting SST asset "writeOnce," with data "@@013@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, writeOnce_set_length_1, writeOnce_set_data_1,
                            PSA_STORAGE_FLAG_WRITE_ONCE);
    if (sst_status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("psa_ps_set() expected PSA_ERROR_NOT_PERMITTED.");
        return;
    }
    /\* Resetting SST asset "writeOnce," with data "@@014@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, writeOnce_set_length_2, writeOnce_set_data_2,
                            PSA_STORAGE_FLAG_WRITE_ONCE);
    if (sst_status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("psa_ps_set() expected PSA_ERROR_NOT_PERMITTED.");
        return;
    }


    /* Removing assets left over from testing: */
    psa_ps_remove\(@@@001@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
