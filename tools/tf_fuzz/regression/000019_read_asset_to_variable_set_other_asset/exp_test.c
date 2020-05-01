/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Test purpose:
 *     to read an asset into a named variable the set another asset from that variable
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
#include "crypto_tests_common.h"
#include "tfm_memory_utils.h"

/* This is not yet right for how to run a test;  need to register tests, etc. */

void test_thread (struct test_result_t *ret) {
    psa_status_t crypto_status;  // result from Crypto calls
    psa_status_t sst_status;

    /* To prevent unused variable warning, as the variable might not be used
     * in this testcase
     */
    (void)sst_status;

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to read an asset into a named variable the set another asset from that variable");


    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t source_set_data\[\] = "@@001@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t source_set_length = @@@004@@@;
    static uint8_t source_exp_data\[\] = "@@001@10@@[a-z\ ]*[\.\?\!]";
    static uint8_t transfer_data\[2048\] = "[A-Z][a-z ]*[\.\?\!]";
    static size_t transfer_length = \d+;
    static uint8_t intermediate_exp_data\[\] = "@@002@10@@[a-z\ ]*[\.\?\!]";
    static uint8_t destination_act_data\[2048\] = "[A-Z][a-z ]*[\.\?\!]";
    static size_t destination_act_length = \d+;


    /* PSA calls to test: */

    /\* Creating SST asset "source," with data "@@001@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, source_set_length, source_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@001@@@, 0, @@@004@@@, transfer_data,
                            &transfer_length);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_get() expected PSA_SUCCESS.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(transfer_data, source_exp_data, 
                   transfer_length) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /\* Creating SST asset "intermediate," with data "@@002@10@@...". \*/
    sst_status = psa_ps_set\(@@@002@@@, transfer_length, transfer_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@002@@@, 0, \d+, transfer_data,
                            &transfer_length);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_get() expected PSA_SUCCESS.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(transfer_data, intermediate_exp_data, 
                   transfer_length) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /\* Creating SST asset "destination," with data "[A-Z][a-z ]*...". \*/
    sst_status = psa_ps_set\(@@@003@@@, transfer_length, transfer_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@003@@@, 0, \d+, destination_act_data,
                            &destination_act_length);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_get() expected PSA_SUCCESS.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(destination_act_data, transfer, 
                   destination_act_length) != 0) {
        TEST_FAIL("Read data should be equal to result data");
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

    /* Test completed */
    ret->val = TEST_PASSED;
}
