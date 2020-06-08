/*
 * Test purpose:
 *     to show a nice party trick
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
    static uint8_t john_set_data\[\] = "@@012@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t john_set_length = \d+;
    static uint8_t paul_set_data\[\] = "@@013@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t paul_set_length = \d+;
    static uint8_t george_set_data\[\] = "@@014@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t george_set_length = \d+;
    static uint8_t and_set_data\[\] = "@@015@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t and_set_length = \d+;
    static uint8_t ringo_set_data\[\] = "@@016@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t ringo_set_length = \d+;
    (void)sst_status;
        /* "void" to prevent unused-variable warning, since the variable may not
         * be used in this particular test case.
         */

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to show a nice party trick");


    /* PSA calls to test: */
    /\* Creating SST asset "john," with data "@@012@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, john_set_length, john_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset "paul," with data "@@013@10@@...". \*/
    sst_status = psa_ps_set\(@@@002@@@, paul_set_length, paul_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset "george," with data "@@014@10@@...". \*/
    sst_status = psa_ps_set\(@@@003@@@, george_set_length, george_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset "and," with data "@@015@10@@...". \*/
    sst_status = psa_ps_set\(@@@004@@@, and_set_length, and_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset "ringo," with data "@@016@10@@...". \*/
    sst_status = psa_ps_set\(@@@005@@@, ringo_set_length, ringo_set_data,
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
    psa_ps_remove\(@@@002@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove\(@@@003@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove\(@@@004@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove\(@@@005@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
