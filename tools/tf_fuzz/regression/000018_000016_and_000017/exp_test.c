/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Test purpose:
 *     to both read the value of a single asset multiple times and assign a sequence of values to a single asset
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

    TEST_LOG("Test to both read the value of a single asset multiple times and assign a sequence of values to a single asset");


    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t indecisive_set_data[] = "First value";
    static uint32_t indecisive_set_length = 11;
    static uint8_t indecisive_exp_data[] = "Not this";
    static uint8_t indecisive_act_data\[2048\] = "[A-Z][a-z ]*[\.\?\!]";
    static size_t indecisive_act_length = \d+;
    static uint8_t indecisive_set_data_1[] = "Second value";
    static uint32_t indecisive_set_length_1 = 12;
    static uint8_t indecisive_exp_data_1[] = "Not this either";
    static uint8_t indecisive_set_data_2\[\] = "@@001@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t indecisive_set_length_2 = \d+;
    static uint8_t indecisive_set_data_3[] = "Fourth value";
    static uint32_t indecisive_set_length_3 = 12;
    static uint8_t indecisive_exp_data_2[] = "No dice on this either";
    static uint8_t indecisive_set_data_4\[\] = "@@002@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t indecisive_set_length_4 = \d+;
    static uint8_t indecisive_set_data_5\[\] = "@@003@10@@[a-z\ ]*[\.\?\!]";
    static uint32_t indecisive_set_length_5 = \d+;


    /* PSA calls to test: */

    /* Creating SST asset "indecisive," with data "First valu...". */
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length, indecisive_set_data,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@001@@@, 0, 11, indecisive_act_data,
                            &indecisive_act_length);
    if (sst_status != fail) {
        TEST_FAIL("psa_ps_get() expected fail.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(indecisive_act_data, indecisive_exp_data, 
                   indecisive_act_length\) != 0\) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /* Resetting SST asset "indecisive," with data "Second val...". */
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length_1, indecisive_set_data_1,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@001@@@, 0, 12, indecisive_act_data,
                            &indecisive_act_length);
    if (sst_status != fail) {
        TEST_FAIL("psa_ps_get() expected fail.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(indecisive_act_data, indecisive_exp_data_1,
                   indecisive_act_length) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /\* Resetting SST asset "indecisive," with data "@@001@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length_2, indecisive_set_data_2,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /* Resetting SST asset "indecisive," with data "Fourth val...". */
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length_3, indecisive_set_data_3,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@001@@@, 0, 12, indecisive_act_data,
                            &indecisive_act_length);
    if (sst_status != fail) {
        TEST_FAIL("psa_ps_get() expected fail.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(indecisive_act_data, indecisive_exp_data_2,
                   indecisive_act_length) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /\* Resetting SST asset "indecisive," with data "@@002@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length_4, indecisive_set_data_4,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /\* Resetting SST asset "indecisive," with data "@@003@10@@...". \*/
    sst_status = psa_ps_set\(@@@001@@@, indecisive_set_length_5, indecisive_set_data_5,
                            PSA_STORAGE_FLAG_[A-Z_]+\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    sst_status = psa_ps_get\(@@@001@@@, 0, \d+, indecisive_act_data,
                            &indecisive_act_length);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_get() expected PSA_SUCCESS.");
        return;
    }
    TEST_LOG(indecisive_act_data);


    /* Removing assets left over from testing: */
    psa_ps_remove\(@@@001@@@\);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
