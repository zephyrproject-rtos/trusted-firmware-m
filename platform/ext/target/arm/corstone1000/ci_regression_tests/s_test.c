/*
 * Copyright (c) 2021-22, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "s_test.h"
#include "platform_base_address.h"
#include "firewall.h"
#include "tfm_sp_log.h"

#define DISABLED_TEST 0

enum host_firewall_host_comp_id_t {
  HOST_FCTRL = (0x00u),
  COMP_SYSPERIPH,
  COMP_DBGPERIPH,
  COMP_AONPERIPH,
  COMP_XNVM,
  COMP_CVM,
  COMP_HOSTCPU,
  COMP_EXTSYS0,
  COMP_EXTSYS1,
  COMP_EXPSLV0,
  COMP_EXPSLV1,
  COMP_EXPMST0,
  COMP_EXPMST1,
  COMP_OCVM,
  COMP_DEBUG,
};

const struct extra_tests_t plat_s_t = {
    .test_entry = s_test,
    .expected_ret = EXTRA_TEST_SUCCESS
};

static int test_host_firewall_status(void)
{
    enum fw_lockdown_status_t status;
    uint32_t any_component_id = 2;

    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, any_component_id);
    status = fw_get_lockdown_status();
    if (status != FW_LOCKED) {
        LOG_INFFMT("FAIL: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_host_firewall_external_flash_configurations(void)
{
    enum rgn_mpl_t mpl_rights = 0;
    enum rgn_mpl_t expected_rights = 0;

#if !(PLATFORM_IS_FVP)
    /* External flash */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_EXPMST0);
    fc_select_region(3);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK | RGN_MPL_SECURE_READ_MASK |
                                              RGN_MPL_SECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL1: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
    /* XIP Permissions */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_XNVM);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK |
                              RGN_MPL_SECURE_READ_MASK |
                              RGN_MPL_NONSECURE_READ_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL2: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
#else
    /* Enable the below test when FVP Host Firewall is configured. */
    /*
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_XNVM);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    tfm_log_printf("mpl rights = %d\n\r", mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK |
                  RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK |
                  RGN_MPL_NONSECURE_READ_MASK |
                  RGN_MPL_NONSECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        tfm_log_printf("FAIL1: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
    */
#endif

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_host_firewall_secure_flash_configurations(void)
{
    enum rgn_mpl_t mpl_rights = 0;
    enum rgn_mpl_t expected_rights = 0;

#if !(PLATFORM_IS_FVP)
    /* External flash */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_EXPMST1);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK | RGN_MPL_SECURE_READ_MASK |
                                              RGN_MPL_SECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
#endif

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_bir_programming(void)
{
    /* BIR is expected to bhaive like write once register */

    volatile uint32_t *bir_base = (uint32_t *)CORSTONE1000_HOST_BIR_BASE;

    bir_base[0] = 0x1;
    bir_base[0] = 0x2;
    if (bir_base[0] != 0x1) {
        LOG_INFFMT("FAIL: %s : (%u)\n\r", __func__, bir_base[0]);
        return EXTRA_TEST_FAILED;
    }

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

int32_t s_test(void)
{
    int status;
    int failures = 0;

#if (DISABLED_TEST == 1)
    status = test_host_firewall_status();
    if (status) {
        failures++;
    }
#endif

    status = test_host_firewall_secure_flash_configurations();
    if (status) {
        failures++;
    }

    status = test_host_firewall_external_flash_configurations();
    if (status) {
        failures++;
    }

#if (DISABLED_TEST == 1)
    status = test_bir_programming();
    if (status) {
        failures++;
    }
#endif

    if (failures) {
        LOG_INFFMT("Not all platform test could pass: failures=%d\n\r", failures);
        return EXTRA_TEST_FAILED;
    }

    LOG_INFFMT("ALL_PASS: corstone1000 platform test cases passed.\n\r");
    return EXTRA_TEST_SUCCESS;
}

int32_t extra_tests_init(struct extra_tests_t *internal_test_t)
{
    /* Add platform init code here. */

    return register_extra_tests(internal_test_t, &plat_s_t);
}
