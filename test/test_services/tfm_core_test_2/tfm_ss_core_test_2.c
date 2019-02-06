/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "tfm_ss_core_test_2.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "spm_partition_defs.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"

#define INVALID_NS_CLIENT_ID  0x49abcdef

/* Don't initialise caller_partition_id_zi and expect it to be linked in the
 * zero-initialised data area
 */
static int32_t caller_client_id_zi;

/* Initialise caller_partition_id_rw and expect it to be linked in the
 * read-write data area
 */
static int32_t caller_client_id_rw = INVALID_NS_CLIENT_ID;

static int32_t* invalid_addresses [] = {(int32_t*)0x0, (int32_t*)0xFFF12000};

/* FIXME: Add a testcase to test that a failed init makes the secure partition
 * closed, and none of its functions can be called.
 * A new test service for this purpose is to be added.
 */
psa_status_t core_test_2_init(void)
{
    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_slave_service(struct psa_invec *in_vec,
                                      size_t in_len,
                                      struct psa_outvec *out_vec,
                                      size_t out_len)
{
    /* This function doesn't do any sanity check on the input parameters, nor
     * makes any expectation of them, always returns successfully, with a
     * non-zero return value.
     * This is to test the parameter sanitization mechanisms implemented in SPM,
     * and the handling of non-zero success codes.
     */

    return CORE_TEST_ERRNO_SUCCESS_2;
}

psa_status_t spm_core_test_2_check_caller_client_id(struct psa_invec *in_vec,
                                    size_t in_len,
                                    struct psa_outvec *out_vec,
                                    size_t out_len)
{
    size_t i;
    int32_t caller_client_id_stack = INVALID_NS_CLIENT_ID;
    int32_t ret;

    caller_client_id_zi = INVALID_NS_CLIENT_ID;

    /* test with invalid output pointers */
    for (i = 0; i < sizeof(invalid_addresses)/sizeof(invalid_addresses[0]); ++i)
    {
        ret = tfm_core_get_caller_client_id(invalid_addresses[i]);
        if (ret != TFM_ERROR_INVALID_PARAMETER) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
    }

    /* test with valid output pointers */
    ret = tfm_core_get_caller_client_id(&caller_client_id_zi);
    if (ret != TFM_SUCCESS || caller_client_id_zi != TFM_SP_CORE_TEST_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_rw);
    if (ret != TFM_SUCCESS || caller_client_id_rw != TFM_SP_CORE_TEST_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_stack);
    if (ret != TFM_SUCCESS ||
            caller_client_id_stack != TFM_SP_CORE_TEST_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_get_every_second_byte(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    int i, j;

    if (in_len != out_len) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }
    for (i = 0; i < in_len; ++i) {
        if (in_vec[i].len/2 > out_vec[i].len) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
        }
        for (j = 1; j < in_vec[i].len; j += 2) {
            ((uint8_t *)out_vec[i].base)[j/2] = ((uint8_t *)in_vec[i].base)[j];
        }
        out_vec[i].len = in_vec[i].len/2;
    }
    return CORE_TEST_ERRNO_SUCCESS;
}

/* Invert function */
#define SFN_INVERT_MAX_LEN 128

psa_status_t spm_core_test_2_sfn_invert(struct psa_invec *in_vec, size_t in_len,
                                   struct psa_outvec *out_vec, size_t out_len)
{
    int32_t i;
    static uint32_t invert_buffer[SFN_INVERT_MAX_LEN];
    int32_t len;
    uint32_t *in_ptr;
    uint32_t *out_ptr;
    int32_t *res_ptr;

    if (in_len != 1 || out_len != 2) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    if ((out_vec[0].len < in_vec[0].len) || (in_vec[0].len%4 != 0) ||
        (out_vec[1].len < sizeof(int32_t))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    len = in_vec[0].len / 4;

    in_ptr = (uint32_t *)in_vec[0].base;
    out_ptr = (uint32_t *)out_vec[0].base;
    res_ptr = (int32_t *)out_vec[1].base;

    if (tfm_core_memory_permission_check(res_ptr, sizeof(int32_t),
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_INVALID_BUFFER;;
    }

    *res_ptr = -1;

    if (len > SFN_INVERT_MAX_LEN) {
        return CORE_TEST_ERRNO_INVALID_BUFFER;;
    }

    /* Check requires byte-based size */
    if ((tfm_core_memory_permission_check(in_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS) ||
        (tfm_core_memory_permission_check(out_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS)) {
        return CORE_TEST_ERRNO_INVALID_BUFFER;;
    }

    for (i = 0; i < len; i++) {
        invert_buffer[i] = in_ptr[i];
    }
    for (i = 0; i < len; i++) {
        invert_buffer[i] = ~invert_buffer[i];
    }
    for (i = 0; i < len; i++) {
        out_ptr[i] = invert_buffer[i];
    }

    *res_ptr = 0;
    return CORE_TEST_ERRNO_SUCCESS;
}
