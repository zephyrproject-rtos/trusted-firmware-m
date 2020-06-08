/*
 * Test purpose:
 *     same with UIDs
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
    static uint8_t SST_ID_17_set_data\[\] = "@@012@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_17_set_length = \d+;
    static uint8_t SST_ID_19_set_data\[\] = "@@013@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_19_set_length = \d+;
    static uint8_t SST_ID_24_set_data\[\] = "@@014@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_24_set_length = \d+;
    static uint8_t SST_ID_31_set_data\[\] = "@@015@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_31_set_length = \d+;
    static uint8_t SST_ID_34_set_data\[\] = "@@016@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_34_set_length = \d+;
    static uint8_t SST_ID_41_set_data\[\] = "@@017@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t SST_ID_41_set_length = \d+;
    (void)sst_status;
        /* "void" to prevent unused-variable warning, since the variable may not
         * be used in this particular test case.
         */

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test same with UIDs");


    /* PSA calls to test: */
    /\* Creating SST asset UID = 17 with data "@@012@10@@...". \*/
    sst_status = psa_ps_set(17, SST_ID_17_set_length, SST_ID_17_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset UID = 19 with data "@@013@10@@...". \*/
    sst_status = psa_ps_set(19, SST_ID_19_set_length, SST_ID_19_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset UID = 24 with data "@@014@10@@...". \*/
    sst_status = psa_ps_set(24, SST_ID_24_set_length, SST_ID_24_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset UID = 31 with data "@@015@10@@...". \*/
    sst_status = psa_ps_set(31, SST_ID_31_set_length, SST_ID_31_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset UID = 34 with data "@@016@10@@...". \*/
    sst_status = psa_ps_set(34, SST_ID_34_set_length, SST_ID_34_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /\* Creating SST asset UID = 41 with data "@@017@10@@...". \*/
    sst_status = psa_ps_set(41, SST_ID_41_set_length, SST_ID_41_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }


    /* Removing assets left over from testing: */
    psa_ps_remove(17);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(19);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(24);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(31);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(34);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(41);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
