/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#ifndef __TFM_SFID_LIST_INC__
#define __TFM_SFID_LIST_INC__

    /******** TFM_SP_STORAGE ********/
    {sst_am_get_handle, SST_AM_GET_HANDLE_SFID},
    {sst_am_create, SST_AM_CREATE_SFID},
    {sst_am_get_info, SST_AM_GET_INFO_SFID},
    {sst_am_get_attributes, SST_AM_GET_ATTRIBUTES_SFID},
    {sst_am_set_attributes, SST_AM_SET_ATTRIBUTES_SFID},
    {sst_am_read, SST_AM_READ_SFID},
    {sst_am_write, SST_AM_WRITE_SFID},
    {sst_am_delete, SST_AM_DELETE_SFID},

    /******** TFM_SP_AUDIT_LOG ********/
    {log_core_retrieve, TFM_LOG_RETRIEVE_SFID},
    {log_core_add_line, TFM_LOG_ADD_LINE_SFID},
    {log_core_get_info, TFM_LOG_GET_INFO_SFID},
    {log_core_delete_items, TFM_LOG_DELETE_ITEMS_SFID},

#ifdef TFM_PARTITION_TEST_CORE
    /******** TFM_SP_CORE_TEST ********/
    {spm_core_test_sfn, TFM_CORE_TEST_SFN_SFID},
    {spm_core_test_sfn_init_success, TFM_CORE_TEST_SFN_INIT_SUCCESS_SFID},
    {spm_core_test_sfn_direct_recursion, TFM_CORE_TEST_SFN_DIRECT_RECURSION_SFID},
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
    /******** TFM_SP_CORE_TEST_2 ********/
    {spm_core_test_2_slave_service, TFM_CORE_TEST_2_SFN_SLAVE_SERVICE_SFID},
    {spm_core_test_2_sfn_invert, TFM_CORE_TEST_2_SFN_INVERT_SFID},
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
    /******** TFM_SP_SST_TEST_PARTITION ********/
    {sst_test_service_sfn_setup, TFM_SST_TEST_SFN_SETUP_SFID},
    {sst_test_service_sfn_dummy_encrypt, TFM_SST_TEST_SFN_DUMMY_ENCRYPT_SFID},
    {sst_test_service_sfn_dummy_decrypt, TFM_SST_TEST_SFN_DUMMY_DECRYPT_SFID},
    {sst_test_service_sfn_clean, TFM_SST_TEST_SFN_CLEAN_SFID},
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    /******** TFM_SP_SECURE_TEST_PARTITION ********/
    {tfm_secure_client_service_sfn_run_tests, TFM_SECURE_CLIENT_SFN_RUN_TESTS_SFID},
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#endif /* __TFM_SFID_LIST_INC__ */
