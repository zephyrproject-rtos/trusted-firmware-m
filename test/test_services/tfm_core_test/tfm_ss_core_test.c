/*
 * Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_ss_core_test.h"
#include "tfm_api.h"
#include "core_test_defs.h"
#include "tfm_ss_core_test_veneers.h"
#include "test/test_services/tfm_core_test_2/tfm_ss_core_test_2_veneers.h"
#include "secure_fw/core/secure_utilities.h"
#include "tfm_secure_api.h"

#include "smm_mps2.h"

static int32_t partition_init_done;

int32_t core_test_init(void)
{
    partition_init_done = 1;
    return TFM_SUCCESS;
}


int32_t spm_core_test_sfn_init_success(void)
{
    if (partition_init_done) {
        return CORE_TEST_ERRNO_SUCCESS;
    } else {
        return CORE_TEST_ERRNO_SP_NOT_INITED;
    }
}

int32_t spm_core_test_sfn_direct_recursion(int32_t depth)
{
    if (depth != 0) {
        /* Protect against scenario where TF-M core fails to block recursion */
        return CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED;
    }
    /* Call to the same service again, should be rejected */
    int32_t ret = tfm_core_test_sfn_direct_recursion(++depth);

    if (ret == TFM_SUCCESS) {
        /* This is an unexpected return value */
        return CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    } else if (ret == CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED) {
        /* This means that service was started in recursion */
        return CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED;
    } else {
        return CORE_TEST_ERRNO_SUCCESS;
    }
}


/* Service RW data array for testing memory accesses */
static int32_t mem[4] = {1, 2, 3, 4};

#define MPS2_USERLED_MASK   (0x3)

int32_t test_mpu_access(
    uint32_t *data_r_ptr, uint32_t *code_ptr, uint32_t *data_w_ptr)
{
    /* If these accesses fail, TFM Core kicks in, there's no returning to sfn */
    /* Code execution, stack access is implicitly tested */
    /* Read RW data */
    int32_t len = sizeof(uint32_t);
    int32_t tmp = mem[0];

    tmp++;
    /* Write to RW data region */
    mem[0] = mem[1];
    mem[1] = tmp;
    mem[2] = len;
    mem[3] = ~len;

    /* Read from NS data region */
    if (tfm_core_memory_permission_check(data_r_ptr, len, TFM_MEMORY_ACCESS_RO)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_INVALID_BUFFER);
    }
    mem[0] = *data_r_ptr;

    /* Write to NS data region */
    if (tfm_core_memory_permission_check(data_w_ptr, len, TFM_MEMORY_ACCESS_RW)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_INVALID_BUFFER);
    }
    data_w_ptr[0] = mem[0];
    data_w_ptr[1] = len;

    /* Read from NS code region */
    if (tfm_core_memory_permission_check(code_ptr, len, TFM_MEMORY_ACCESS_RO)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_INVALID_BUFFER);
    }
    data_w_ptr[2] = *code_ptr;

    return TFM_SUCCESS;
}

int32_t test_memory_permissions(
    uint32_t *data_r_ptr, uint32_t *code_ptr, uint32_t *data_w_ptr)
{
    int32_t len = sizeof(uint32_t);
    /* Read from NS data region */
    if (tfm_core_memory_permission_check(data_r_ptr, len, TFM_MEMORY_ACCESS_RO)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    /* Write to NS data region */
    if (tfm_core_memory_permission_check(data_w_ptr, len, TFM_MEMORY_ACCESS_RW)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    /* Read from NS code region */
    if (tfm_core_memory_permission_check(code_ptr, len, TFM_MEMORY_ACCESS_RO)
        != TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    /* Write to NS code region - should be rejected */
    if (tfm_core_memory_permission_check(code_ptr, len, TFM_MEMORY_ACCESS_RW)
        == TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    /* NS peripheral region - should be rejected */
    if (tfm_core_memory_permission_check(
        (uint32_t *)0x40000000, len, TFM_MEMORY_ACCESS_RO) == TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    /* S peripheral region - should be rejected */
    if (tfm_core_memory_permission_check(
        (uint32_t *)0x50000000, len, TFM_MEMORY_ACCESS_RO) == TFM_SUCCESS) {
        CORE_TEST_RETURN_ERROR(CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR);
    }

    return TFM_SUCCESS;
}

int32_t test_share_redirection(void)
{
    uint32_t tmp;

    if (tfm_core_set_buffer_area(TFM_BUFFER_SHARE_SCRATCH) != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }
    /* Read from scratch */
    tmp = tfm_scratch_area[0];
    /* Write to scratch */
    tfm_scratch_area[1] = tmp;

    return TFM_SUCCESS;
}

int32_t test_peripheral_access(void)
{
    struct arm_mps2_fpgaio_t *fpgaio = SEC_MPS2_FPGAIO;
    /* Check read access */
    uint32_t leds = fpgaio->LED;
    /* Write access */
    fpgaio->LED = ~leds;
    /* Check result of write access, only compare 2 valid bits */
    uint32_t invleds = fpgaio->LED;

    if ((invleds & MPS2_USERLED_MASK) != (~leds & MPS2_USERLED_MASK)) {
        /* Code failed to invert value in peripheral reg */
        return CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED;
    }

    return TFM_SUCCESS;
}

#define SS_BUFFER_LEN 16

int32_t test_ss_to_ss_buffer(uint32_t *in_ptr, uint32_t *out_ptr, int32_t len)
{
    int32_t i;
    /* Service internal buffer */
    uint32_t ss_buffer[SS_BUFFER_LEN];
    /* Slave service has to use scratch area */
    uint32_t *slave_buffer = (uint32_t *)tfm_scratch_area;
    /* Store result at end of scratch area to test entire range for RW access */
    int32_t *result_ptr = (int32_t *)&tfm_scratch_area[tfm_scratch_area_size-4];

    if (len > SS_BUFFER_LEN) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    /* Check requires byte-based size */
    if ((tfm_core_memory_permission_check(in_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS) ||
        (tfm_core_memory_permission_check(out_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS)) {
        return CORE_TEST_ERRNO_INVALID_BUFFER;
    }

    for (i = 0; i < len; i++) {
        ss_buffer[i] = in_ptr[i];
    }

    if (tfm_core_set_buffer_area(TFM_BUFFER_SHARE_SCRATCH) != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }

    for (i = 0; i < len; i++) {
        slave_buffer[i] = ss_buffer[i];
    }

    /* Call internal service with buffer handling */
    int32_t res = tfm_core_test_2_sfn_invert(result_ptr, slave_buffer,
                                         slave_buffer, len);

    if (res != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }

    for (i = 0; i < len; i++) {
        if (slave_buffer[i] != ~ss_buffer[i]) {
            return CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE;
        }
        ss_buffer[i] = slave_buffer[i];
    }

    if (tfm_core_set_buffer_area(TFM_BUFFER_SHARE_DEFAULT) != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }

    for (i = 0; i < len; i++) {
        out_ptr[i] = ss_buffer[i];
    }

    return TFM_SUCCESS;
}


static int32_t test_ss_to_ss(void)
{
    /* Call to a different service, should be sucessful */
    int32_t ret = tfm_core_test_2_veneer_slave_service();

    if (ret == TFM_SUCCESS) {
        return CORE_TEST_ERRNO_SUCCESS;
    } else {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }
}

#ifdef CORE_TEST_INTERACTIVE
#define MPS2_USERPB0_BASE   (0x50302008)
#define MPS2_USERPB0_MASK   (0x1)

static void wait_button_event(void)
{
    volatile uint32_t *p_btn = (volatile uint32_t *) MPS2_USERPB0_BASE;
    *p_btn = *p_btn;

    /* Wait until user button 0 is pressed */
    while (!(*p_btn & MPS2_USERPB0_MASK)) {
      ;
    }

    /*
     * The follow wait is required to skip multiple continues in one go due to
     * the fast execution of the code and time used by the user to
     * release button.
     */

    /* Wait until user button 0 is released */
    while ((*p_btn & MPS2_USERPB0_MASK)) {
      ;
    }
}

int32_t test_wait_button(void)
{
    LOG_MSG("Inside the service, press button to continue...");
    wait_button_event();
    LOG_MSG("Leaving the service");
    return TFM_SUCCESS;
}
#endif

static int32_t test_block(int32_t arg1, int32_t arg2, int32_t arg3)
{
#ifdef CORE_TEST_INTERACTIVE
    /* Only block if interactive test is turned on */
    return test_wait_button();
#else
    /* This test should not be run if interactive tests are disabled */
    return CORE_TEST_ERRNO_TEST_FAULT;
#endif /* CORE_TEST_INTERACTIVE */
}

int32_t spm_core_test_sfn(int32_t tc, int32_t arg1, int32_t arg2, int32_t arg3)
{
    switch (tc) {
    case CORE_TEST_ID_MPU_ACCESS:
        return test_mpu_access(
            (uint32_t *)arg1, (uint32_t *)arg2, (uint32_t *)arg3);
    case CORE_TEST_ID_MEMORY_PERMISSIONS:
        return test_memory_permissions(
            (uint32_t *)arg1, (uint32_t *)arg2, (uint32_t *)arg3);
    case CORE_TEST_ID_SHARE_REDIRECTION:
        return test_share_redirection();
    case CORE_TEST_ID_SS_TO_SS:
        return test_ss_to_ss();
    case CORE_TEST_ID_SS_TO_SS_BUFFER:
        return test_ss_to_ss_buffer((uint32_t *)arg1, (uint32_t *)arg2, arg3);
    case CORE_TEST_ID_PERIPHERAL_ACCESS:
        return test_peripheral_access();
    case CORE_TEST_ID_BLOCK:
        return test_block(arg1, arg2, arg3);
    case CORE_TEST_ID_NS_THREAD:
    case CORE_TEST_ID_NS_SVC:
    case CORE_TEST_ID_TWO_SFN_ONE_SVC:
        /* dummy service call is enough */
        return CORE_TEST_ERRNO_SUCCESS;
    default:
        return CORE_TEST_ERRNO_INVALID_TEST_ID;
    }
}
