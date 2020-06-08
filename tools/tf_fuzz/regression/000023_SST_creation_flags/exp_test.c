/*
 * Test purpose:
 *     to set SST creation flags
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
    static uint8_t hasCF_set_data[] = "this asset has write-once";
    static uint32_t hasCF_set_length = 25;
    static uint8_t hasCF2_set_data[] = "this asset has also write-once";
    static uint32_t hasCF2_set_length = 30;
    static uint8_t alsoHasCF_set_data[] = "this asset has no flags";
    static uint32_t alsoHasCF_set_length = 23;
    static uint8_t alsoHasCF2_set_data[] = "this asset has NO_CONFIDENTIALITY";
    static uint32_t alsoHasCF2_set_length = 33;
    static uint8_t yetAnother_set_data[] = "this asset has NO_REPLAY_PROTECTION";
    static uint32_t yetAnother_set_length = 35;
    (void)sst_status;
        /* "void" to prevent unused-variable warning, since the variable may not
         * be used in this particular test case.
         */

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to set SST creation flags");


    /* PSA calls to test: */
    /* Creating SST asset "hasCF," with data "this asset...". */
    sst_status = psa_ps_set\(@@@001@@@, hasCF_set_length, hasCF_set_data,
                            PSA_STORAGE_FLAG_WRITE_ONCE);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /* Creating SST asset "hasCF2," with data "this asset...". */
    sst_status = psa_ps_set\(@@@002@@@, hasCF2_set_length, hasCF2_set_data,
                            PSA_STORAGE_FLAG_WRITE_ONCE);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /* Creating SST asset "alsoHasCF," with data "this asset...". */
    sst_status = psa_ps_set\(@@@003@@@, alsoHasCF_set_length, alsoHasCF_set_data,
                            PSA_STORAGE_FLAG_NONE);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /* Creating SST asset "alsoHasCF2," with data "this asset...". */
    sst_status = psa_ps_set\(@@@004@@@, alsoHasCF2_set_length, alsoHasCF2_set_data,
                            PSA_STORAGE_FLAG_NO_CONFIDENTIALITY);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }
    /* Creating SST asset "yetAnother," with data "this asset...". */
    sst_status = psa_ps_set\(@@@005@@@, yetAnother_set_length, yetAnother_set_data,
                            PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION);
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
