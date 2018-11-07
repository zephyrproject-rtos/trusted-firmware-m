/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#ifndef __TFM_SFID_LIST_INC__
#define __TFM_SFID_LIST_INC__

    /******** TFM_SP_STORAGE ********/
    {sst_am_create, SST_AM_CREATE_SFID},
    {sst_am_get_info, SST_AM_GET_INFO_SFID},
    {sst_am_get_attributes, SST_AM_GET_ATTRIBUTES_SFID},
    {sst_am_set_attributes, SST_AM_SET_ATTRIBUTES_SFID},
    {sst_am_read, SST_AM_READ_SFID},
    {sst_am_write, SST_AM_WRITE_SFID},
    {sst_am_delete, SST_AM_DELETE_SFID},

    /******** TFM_SP_AUDIT_LOG ********/
    {audit_core_retrieve_record, TFM_AUDIT_RETRIEVE_RECORD_SFID},
    {audit_core_add_record, TFM_AUDIT_ADD_RECORD_SFID},
    {audit_core_get_info, TFM_AUDIT_GET_INFO_SFID},
    {audit_core_get_record_info, TFM_AUDIT_GET_RECORD_INFO_SFID},
    {audit_core_delete_record, TFM_AUDIT_DELETE_RECORD_SFID},

    /******** TFM_SP_CRYPTO ********/
    {tfm_crypto_import_key, TFM_CRYPTO_IMPORT_KEY_SFID},
    {tfm_crypto_destroy_key, TFM_CRYPTO_DESTROY_KEY_SFID},
    {tfm_crypto_get_key_information, TFM_CRYPTO_GET_KEY_INFORMATION_SFID},
    {tfm_crypto_export_key, TFM_CRYPTO_EXPORT_KEY_SFID},
    {tfm_crypto_cipher_set_iv, TFM_CRYPTO_CIPHER_SET_IV_SFID},
    {tfm_crypto_cipher_encrypt_setup, TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SFID},
    {tfm_crypto_cipher_decrypt_setup, TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SFID},
    {tfm_crypto_cipher_update_wrapper, TFM_CRYPTO_CIPHER_UPDATE_SFID},
    {tfm_crypto_cipher_abort, TFM_CRYPTO_CIPHER_ABORT_SFID},
    {tfm_crypto_cipher_finish, TFM_CRYPTO_CIPHER_FINISH_SFID},
    {tfm_crypto_hash_setup, TFM_CRYPTO_HASH_SETUP_SFID},
    {tfm_crypto_hash_update, TFM_CRYPTO_HASH_UPDATE_SFID},
    {tfm_crypto_hash_finish, TFM_CRYPTO_HASH_FINISH_SFID},
    {tfm_crypto_hash_verify, TFM_CRYPTO_HASH_VERIFY_SFID},
    {tfm_crypto_hash_abort, TFM_CRYPTO_HASH_ABORT_SFID},
    {tfm_crypto_mac_sign_setup, TFM_CRYPTO_MAC_SIGN_SETUP_SFID},
    {tfm_crypto_mac_verify_setup, TFM_CRYPTO_MAC_VERIFY_SETUP_SFID},
    {tfm_crypto_mac_update, TFM_CRYPTO_MAC_UPDATE_SFID},
    {tfm_crypto_mac_sign_finish, TFM_CRYPTO_MAC_SIGN_FINISH_SFID},
    {tfm_crypto_mac_verify_finish, TFM_CRYPTO_MAC_VERIFY_FINISH_SFID},
    {tfm_crypto_mac_abort, TFM_CRYPTO_MAC_ABORT_SFID},
    {tfm_crypto_aead_decrypt_wrapper, TFM_CRYPTO_AEAD_DECRYPT_SFID},
    {tfm_crypto_aead_encrypt_wrapper, TFM_CRYPTO_AEAD_ENCRYPT_SFID},

    /******** TFM_SP_PLATFORM ********/
    {platform_sp_system_reset, TFM_SP_PLATFORM_SYSTEM_RESET_SFID},

    /******** TFM_SP_INITIAL_ATTESTATION ********/
    {initial_attest_get_token, TFM_ATTEST_GET_TOKEN_SFID},

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
    {spm_core_test_2_check_caller_client_id, TFM_CORE_TEST_2_SFN_CHECK_CALLER_CLIENT_ID_SFID},
    {spm_core_test_2_get_every_second_byte, TFM_CORE_TEST_2_SFN_GET_EVERY_SECOND_BYTE},
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
