/*
 * Test purpose:
 *     to show that if you don't have say anything about data to randomize it
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
    static uint8_t random_set_data\[\] = "@@002@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t random_set_length = \d+;
    static uint8_t @@@003@@@_set_data\[\] = "@@004@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t @@@003@@@_set_length = \d+;
    (void)sst_status;
        /* "void" to prevent unused-variable warning, since the variable may not
         * be used in this particular test case.
         */

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to show that if you don't have say anything about data to randomize it");


    /* PSA calls to test: */
    /\* Creating SST asset "random," with data "@@002@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, random_set_length, random_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset "@@@003@@@," with data "@@004@10@@...". \*/
    sst_status = psa_ps_set\(@@@004@@@, @@@003@@@_set_length, @@@003@@@_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }


    /* Removing assets left over from testing: */
    psa_ps_remove\(@@@001@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove\(@@@004@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
