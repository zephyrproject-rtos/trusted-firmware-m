/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rse_test_common.h"

#include "test_state_transitions.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))


static struct conditional_test_t state_transitions[] = {
    {
        .tp_mode = LCM_TP_MODE_VIRGIN,
        .any_lcs = true,
        .any_sp_state = true,
        .test = {
            &rse_test_virgin_to_tci_or_pci,
            "RSE_BL1_1_TRANSITIONING_TO_TCI/PCI",
            "Provisioning transition from Virgin TP mode to TCI or PCI mode"
        },
    },
    {
        .any_tp_mode = true,
        .lcs = LCM_LCS_CM,
        .sp_enabled = LCM_FALSE,
        .test = {
            &rse_test_cm_sp_enable,
            "RSE_BL1_1_TRANSITIONING_TO_CM_SP_ENABLE",
            "Provisioning transition to CM LCS secure provisioning enable mode"
        },
    },
    {
        .any_tp_mode = true,
        .lcs = LCM_LCS_CM,
        .sp_enabled = LCM_TRUE,
        .test = {
            &rse_test_cm_to_dm,
            "RSE_BL1_1_TRANSITIONING_TO_DM",
            "Provisioning transition to DM LCS"
        },
    },
    {
        .any_tp_mode = true,
        .lcs = LCM_LCS_DM,
        .sp_enabled = LCM_FALSE,
        .test = {
            &rse_test_dm_sp_enable,
            "RSE_BL1_1_TRANSITIONING_TO_DM_SP_ENABLE",
            "Provisioning transition to DM LCS secure provisioning enable mode"
        },
    },
    {
        .any_tp_mode = true,
        .lcs = LCM_LCS_DM,
        .sp_enabled = LCM_TRUE,
        .test = {
            &rse_test_dm_to_se,
            "RSE_BL1_1_TRANSITIONING_TO_SE",
            "Provisioning transition to SE LCS"
        },
    },
    {
        .any_tp_mode = true,
        .lcs = LCM_LCS_SE,
        .any_sp_state = true,
        .test = {
            &rse_test_se_to_rma,
            "RSE_BL1_1_TRANSITIONING_TO_RMA",
            "Provisioning transition to RMA LCS"
        },
    },
};

static struct test_t bl1_1_extra_tests[100];

void register_testsuite_extra_bl1_1(struct test_suite_t *p_test_suite)
{
    set_testsuite("RSE Tests", bl1_1_extra_tests, 0, p_test_suite);

    /* This one must be added last in order for the state transitions to work */
    add_conditional_tests_to_testsuite(state_transitions, ARRAY_SIZE(state_transitions),
                                       p_test_suite, ARRAY_SIZE(bl1_1_extra_tests));
}
