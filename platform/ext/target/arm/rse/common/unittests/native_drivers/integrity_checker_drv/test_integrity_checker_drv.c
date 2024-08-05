/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "integrity_checker_drv.h"
#include "tfm_hal_device_header.h"

#include "unity.h"

/*
 * RSE build IC configuration values:
 *  ZCA             = 0b1 -> Zero count algorithm is supported.
 *  ICS_ZCA         = 0b1 -> Compute mode for zero count algorithm
 *                           is supported.
 *  BUFFSIZE        = 0x4 -> Buffer size of the integrity checker
 *                           is 128 bytes
 */
#define IC_BC_RESET_VALUE 0x109u   /* Build configuration register */
#define IC_C_RESET_VALUE 0x0u      /* Control register */
#define IC_IS_RESET_VALUE 0x0u     /* Interrupt status register */
#define IC_IE_RESET_VALUE 0x0u     /* Interrupt enable register */
#define IC_AE_RESET_VALUE 0x0u     /* Alarm enable register */
#define IC_IC_RESET_VALUE 0x0u     /* Interrupt clear register */
#define IC_DA_RESET_VALUE 0x0u     /* Data address register */
#define IC_DL_RESET_VALUE 0x0u     /* Data length register */
#define IC_EVA_RESET_VALUE 0x0u    /* Expected value address register */
#define IC_CVA_RESET_VALUE 0x0u    /* Computed value address register */
#define IC_CVAL_RESET_VALUE 0x0u   /* Computed value register */
#define IC_PIDR0_RESET_VALUE 0xFDu /* Peripheral ID 0 register */
#define IC_PIDR1_RESET_VALUE 0xB0u /* Peripheral ID 1 register */
#define IC_PIDR2_RESET_VALUE 0xBu  /* Peripheral ID 2 register */
#define IC_PIDR3_RESET_VALUE 0x0u  /* Peripheral ID 3 register */
#define IC_PIDR4_RESET_VALUE 0x4u  /* Peripheral ID 4 register */
#define IC_CIDR0_RESET_VALUE 0xDu  /* Component ID 0 register */
#define IC_CIDR1_RESET_VALUE 0xF0u /* Component ID 1 register */
#define IC_CIDR2_RESET_VALUE 0x5u  /* Component ID 2 register */
#define IC_CIDR3_RESET_VALUE 0xB1u /* Component ID 3 register */

struct _integrity_checker_reg_map_t {
    volatile uint32_t icbc;
    /*!< Offset: 0x000 (R/ ) Integrity Checker Build
     *                       Configuration Register */
    volatile uint32_t icc;
    /*!< Offset: 0x004 (R/W) Integrity Checker
     *                       Control Register */
    volatile uint32_t icis;
    /*!< Offset: 0x008 (R/ ) Integrity Checker Interrupt
     *                       Status Register */
    volatile uint32_t icie;
    /*!< Offset: 0x00C (R/W) Integrity Checker Interrupt
     *                       Enable Register */
    volatile uint32_t icae;
    /*!< Offset: 0x010 (R/W) Integrity Checker Alarm
     *                       Enable Register */
    volatile uint32_t icic;
    /*!< Offset: 0x014 (R/W) Integrity Checker Interrupt
     *                       Clear Register */
    volatile uint32_t icda;
    /*!< Offset: 0x018 (R/W) Integrity Checker Data
     *                       Address Register */
    volatile uint32_t icdl;
    /*!< Offset: 0x01C (R/W) Integrity Checker Data
     *                       Length Register */
    volatile uint32_t iceva;
    /*!< Offset: 0x020 (R/W) Integrity Checker Expected
     *                       Value Address Register */
    volatile uint32_t iccva;
    /*!< Offset: 0x024 (R/W) Integrity Checker Computed
     *                       Value Address Register */
    volatile uint32_t iccval[8];
    /*!< Offset: 0x028 (R/ ) Integrity Checker Computed
     *                       Value Register */
    volatile uint32_t reserved_0[0x3E1];
    /*!< Offset: 0x48-0xFCC Reserved */
    volatile uint32_t pidr4;
    /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved_1[3];
    /*!< Offset: 0xFD4-0xFDC Reserved */
    volatile uint32_t pidr0;
    /*!< Offset: 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;
    /*!< Offset: 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;
    /*!< Offset: 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;
    /*!< Offset: 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;
    /*!< Offset: 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;
    /*!< Offset: 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;
    /*!< Offset: 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;
    /*!< Offset: 0xFFC (R/ ) Component ID 3 */
} regmap;

static struct integrity_checker_dev_cfg_t INTEGRITY_CHECKER_DEV_CFG_S = {
    .base = (uintptr_t)&regmap,
};
struct integrity_checker_dev_t INTEGRITY_CHECKER_DEV_S = {
    .cfg = &(INTEGRITY_CHECKER_DEV_CFG_S)};

/* Set IC Registers to their default reset values */
void set_default_regmap_values(void)
{
    regmap.icbc = IC_BC_RESET_VALUE;
    regmap.icc = IC_C_RESET_VALUE;
    regmap.icis = IC_IS_RESET_VALUE;
    regmap.icie = IC_IE_RESET_VALUE;
    regmap.icae = IC_AE_RESET_VALUE;
    regmap.icic = IC_IC_RESET_VALUE;
    regmap.icda = IC_DA_RESET_VALUE;
    regmap.icdl = IC_DL_RESET_VALUE;
    regmap.iceva = IC_EVA_RESET_VALUE;
    regmap.iccva = IC_CVA_RESET_VALUE;
    memset((void *)regmap.iccval, IC_CVAL_RESET_VALUE, sizeof(regmap.iccval));
    memset((void *)regmap.reserved_0, 0x0u, sizeof(regmap.reserved_0));
    regmap.pidr4 = IC_PIDR4_RESET_VALUE;
    memset((void *)regmap.reserved_1, 0x0u, sizeof(regmap.reserved_1));
    regmap.pidr0 = IC_PIDR0_RESET_VALUE;
    regmap.pidr1 = IC_PIDR1_RESET_VALUE;
    regmap.pidr2 = IC_PIDR2_RESET_VALUE;
    regmap.pidr3 = IC_PIDR3_RESET_VALUE;
    regmap.cidr0 = IC_CIDR0_RESET_VALUE;
    regmap.cidr1 = IC_CIDR1_RESET_VALUE;
    regmap.cidr2 = IC_CIDR2_RESET_VALUE;
    regmap.cidr3 = IC_CIDR3_RESET_VALUE;
}

/* Verify that Read-Only registers have not been modified */
void verify_read_only_registers(void)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_BC_RESET_VALUE, regmap.icbc,
                                     "Read-Only register ICBC was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_PIDR0_RESET_VALUE, regmap.pidr0,
                                     "Read-Only register PIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_PIDR1_RESET_VALUE, regmap.pidr1,
                                     "Read-Only register PIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_PIDR2_RESET_VALUE, regmap.pidr2,
                                     "Read-Only register PIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_PIDR3_RESET_VALUE, regmap.pidr3,
                                     "Read-Only register PIDR3 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_PIDR4_RESET_VALUE, regmap.pidr4,
                                     "Read-Only register PIDR4 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_CIDR0_RESET_VALUE, regmap.cidr0,
                                     "Read-Only register CIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_CIDR1_RESET_VALUE, regmap.cidr1,
                                     "Read-Only register CIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_CIDR2_RESET_VALUE, regmap.cidr2,
                                     "Read-Only register CIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(IC_CIDR3_RESET_VALUE, regmap.cidr3,
                                     "Read-Only register CIDR3 was modified");
}

void setUp(void) { set_default_regmap_values(); }

void tearDown(void) { verify_read_only_registers(); }

TEST_CASE(INTEGRITY_CHECKER_MODE_SHA256 + 1,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_UNSUPPORTED_MODE)
TEST_CASE(INTEGRITY_CHECKER_MODE_SHA256, INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_UNSUPPORTED_MODE)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT, 0x0u, 0x0u,
          INTEGRITY_CHECKER_ERROR_COMPUTE_VALUE_BUFFER_TOO_SMALL)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_COMPUTE_VALUE_INVALID_LENGTH)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_ERROR_COMPUTE_VALUE_INVALID_ALIGNMENT)
void test_integrity_checker_compute_value_args(
    enum integrity_checker_mode_t mode, size_t data_size, size_t value_size,
    uintptr_t data_ptr_offset, /* Used to simulate data misalignment */
    enum integrity_checker_error_t expected_err)
{
    enum integrity_checker_error_t ic_err;
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        data[INTEGRITY_CHECKER_REQUIRED_ALIGNMENT / sizeof(uint32_t)];
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        value[INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT / sizeof(uint32_t)];
    size_t value_len;

    /* Act */
    ic_err = integrity_checker_compute_value(&INTEGRITY_CHECKER_DEV_S, mode,
                                             data + data_ptr_offset, data_size,
                                             value, value_size, &value_len);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, ic_err);
}

static void *ic_comparison_hw_op(void *thread_args)
{
    uint32_t icis_mask = *(uint32_t *)thread_args;

    while (!(regmap.icc & 0x1u)) {
    }
    regmap.icc &= ~0x1u;

    regmap.icis |= icis_mask;

    return NULL;
}

TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x4u, 0x0u,
          INTEGRITY_CHECKER_ERROR_COMPUTE_VALUE_OPERATION_FAILED)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT,
          INTEGRITY_CHECKER_ERROR_NONE)
void test_integrity_checker_compute_value(
    enum integrity_checker_mode_t mode, size_t data_size, size_t value_size,
    uint32_t icis_mask, uint32_t expected_value_len,
    enum integrity_checker_error_t expected_err)
{
    enum integrity_checker_error_t ic_err;
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        data[INTEGRITY_CHECKER_REQUIRED_ALIGNMENT / sizeof(uint32_t)];
    uint32_t value[INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT / sizeof(uint32_t)];
    size_t value_len;

    int32_t pthread_err;
    pthread_t pthread_id;
    uint32_t pthread_args;

    uint32_t expected_icc;
    uint32_t expected_icae;
    uint32_t expected_icie;
    uint32_t expected_icic;
    uint32_t expected_icda;
    uint32_t expected_icdl;

    /* Prepare */
    value_len = 0;

    expected_icc = 0x2F0u;
    expected_icae = 0x0u;
    expected_icie = 0xFFu;
    expected_icic = 0xFFu;
    expected_icda = (uintptr_t)data;
    expected_icdl = 0x1u;

    pthread_args = icis_mask;

    /* Act */
    pthread_err =
        pthread_create(&pthread_id, NULL, ic_comparison_hw_op, &pthread_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create ic_comparison_hw_op thread");

    ic_err = integrity_checker_compute_value(&INTEGRITY_CHECKER_DEV_S, mode,
                                             data, data_size, value, value_size,
                                             &value_len);

    pthread_err = pthread_join(pthread_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_err,
                                  "Failed to join ic_comparison_hw_op thread");

    /* Cleanup */
    regmap.icis = IC_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.icc, expected_icc);
    TEST_ASSERT_EQUAL_UINT32(regmap.icae, expected_icae);
    TEST_ASSERT_EQUAL_UINT32(regmap.icie, expected_icie);
    TEST_ASSERT_EQUAL_UINT32(regmap.icic, expected_icic);
    TEST_ASSERT_EQUAL_UINT32(regmap.icdl, expected_icdl);
    TEST_ASSERT_EQUAL_UINT32(regmap.icda, expected_icda);
    TEST_ASSERT_EQUAL_UINT32(value_len, expected_value_len);
    TEST_ASSERT_EQUAL(expected_err, ic_err);
}

TEST_CASE(INTEGRITY_CHECKER_MODE_SHA256 + 1,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_UNSUPPORTED_MODE)
TEST_CASE(INTEGRITY_CHECKER_MODE_SHA256, INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_UNSUPPORTED_MODE)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT, 0x0u, 0x0u,
          INTEGRITY_CHECKER_ERROR_CHECK_VALUE_BUFFER_TOO_SMALL)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x0u,
          INTEGRITY_CHECKER_ERROR_CHECK_VALUE_INVALID_LENGTH)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_ERROR_CHECK_VALUE_INVALID_ALIGNMENT)
void test_integrity_checker_check_value_args(
    enum integrity_checker_mode_t mode, size_t data_size, size_t value_size,
    uintptr_t data_ptr_offset, /* Used to simulate data misalignment */
    enum integrity_checker_error_t expected_err)
{
    enum integrity_checker_error_t ic_err;
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        data[INTEGRITY_CHECKER_REQUIRED_ALIGNMENT / sizeof(uint32_t)];
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        value[INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT / sizeof(uint32_t)];

    /* Act */
    ic_err = integrity_checker_check_value(&INTEGRITY_CHECKER_DEV_S, mode,
                                           data + data_ptr_offset, data_size,
                                           value, value_size);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, ic_err);
}

TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x4u,
          INTEGRITY_CHECKER_ERROR_CHECK_VALUE_OPERATION_FAILED)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x2u,
          INTEGRITY_CHECKER_ERROR_CHECK_VALUE_COMPARISON_FAILED)
TEST_CASE(INTEGRITY_CHECKER_MODE_ZERO_COUNT,
          INTEGRITY_CHECKER_REQUIRED_ALIGNMENT,
          INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT, 0x1u,
          INTEGRITY_CHECKER_ERROR_NONE)
void test_integrity_checker_check_value(
    enum integrity_checker_mode_t mode, size_t data_size, size_t value_size,
    uint32_t icis_mask, enum integrity_checker_error_t expected_err)
{
    enum integrity_checker_error_t ic_err;
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        data[INTEGRITY_CHECKER_REQUIRED_ALIGNMENT / sizeof(uint32_t)];
    uint32_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT)
        value[INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT / sizeof(uint32_t)];

    int32_t pthread_err;
    pthread_t pthread_id;
    uint32_t pthread_args;

    uint32_t expected_icc;
    uint32_t expected_icae;
    uint32_t expected_icie;
    uint32_t expected_icic;
    uint32_t expected_icda;
    uint32_t expected_icdl;

    /* Prepare */
    expected_icc = 0x2E0u;
    expected_icae = 0x0u;
    expected_icie = 0xFFu;
    expected_icic = 0xFFu;
    expected_icda = (uintptr_t)data;
    expected_icdl = 0x1u;

    pthread_args = icis_mask;

    /* Act */
    pthread_err =
        pthread_create(&pthread_id, NULL, ic_comparison_hw_op, &pthread_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create ic_comparison_hw_op thread");

    ic_err = integrity_checker_check_value(&INTEGRITY_CHECKER_DEV_S, mode, data,
                                           data_size, value, value_size);

    pthread_err = pthread_join(pthread_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_err,
                                  "Failed to join ic_comparison_hw_op thread");

    /* Cleanup */
    regmap.icis = IC_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.icc, expected_icc);
    TEST_ASSERT_EQUAL_UINT32(regmap.icae, expected_icae);
    TEST_ASSERT_EQUAL_UINT32(regmap.icie, expected_icie);
    TEST_ASSERT_EQUAL_UINT32(regmap.icic, expected_icic);
    TEST_ASSERT_EQUAL_UINT32(regmap.icdl, expected_icdl);
    TEST_ASSERT_EQUAL_UINT32(regmap.icda, expected_icda);
    TEST_ASSERT_EQUAL(expected_err, ic_err);
}
