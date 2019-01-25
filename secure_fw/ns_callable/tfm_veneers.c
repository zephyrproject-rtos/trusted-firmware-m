/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "secure_fw/spm/spm_partition_defs.h"

/******** TFM_SP_STORAGE ********/
psa_status_t tfm_sst_set_req(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_sst_get_req(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_sst_get_info_req(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_sst_remove_req(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_sst_get_support_req(struct psa_invec *, size_t, struct psa_outvec *, size_t);

/******** TFM_SP_AUDIT_LOG ********/
psa_status_t audit_core_retrieve_record(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t audit_core_add_record(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t audit_core_get_info(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t audit_core_get_record_info(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t audit_core_delete_record(struct psa_invec *, size_t, struct psa_outvec *, size_t);

/******** TFM_SP_CRYPTO ********/
psa_status_t tfm_crypto_import_key(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_destroy_key(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_get_key_information(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_export_key(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_key_policy_init(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_key_policy_set_usage(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_key_policy_get_usage(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_key_policy_get_algorithm(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_set_key_policy(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_get_key_policy(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_get_key_lifetime(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_set_key_lifetime(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_set_iv(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_encrypt_setup(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_decrypt_setup(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_update_wrapper(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_abort(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_cipher_finish(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_hash_setup(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_hash_update(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_hash_finish(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_hash_verify(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_hash_abort(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_sign_setup(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_verify_setup(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_update(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_sign_finish(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_verify_finish(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_mac_abort(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_aead_decrypt_wrapper(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t tfm_crypto_aead_encrypt_wrapper(struct psa_invec *, size_t, struct psa_outvec *, size_t);

/******** TFM_SP_PLATFORM ********/
psa_status_t platform_sp_system_reset(struct psa_invec *, size_t, struct psa_outvec *, size_t);

/******** TFM_SP_INITIAL_ATTESTATION ********/
psa_status_t initial_attest_get_token(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t initial_attest_get_token_size(struct psa_invec *, size_t, struct psa_outvec *, size_t);

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST ********/
psa_status_t spm_core_test_sfn(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t spm_core_test_sfn_init_success(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t spm_core_test_sfn_direct_recursion(struct psa_invec *, size_t, struct psa_outvec *, size_t);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST_2 ********/
psa_status_t spm_core_test_2_slave_service(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t spm_core_test_2_sfn_invert(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t spm_core_test_2_check_caller_client_id(struct psa_invec *, size_t, struct psa_outvec *, size_t);
psa_status_t spm_core_test_2_get_every_second_byte(struct psa_invec *, size_t, struct psa_outvec *, size_t);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
/******** TFM_SP_SECURE_TEST_PARTITION ********/
psa_status_t tfm_secure_client_service_sfn_run_tests(struct psa_invec *, size_t, struct psa_outvec *, size_t);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_SERVICE_TEST ********/
#endif /* TFM_PSA_API */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_CLIENT_TEST ********/
#endif /* TFM_PSA_API */


#define TFM_VENEER_FUNCTION(partition_name, sfn_name) \
    __tfm_secure_gateway_attributes__ \
    psa_status_t tfm_##sfn_name##_veneer(struct psa_invec *in_vec, \
                                         size_t in_len, \
                                         struct psa_outvec *out_vec, \
                                         size_t out_len) \
    { \
        TFM_CORE_IOVEC_SFN_REQUEST(partition_name##_ID, \
                                   sfn_name, \
                                   in_vec, in_len, out_vec, out_len); \
    }

/******** TFM_SP_STORAGE ********/
TFM_VENEER_FUNCTION(TFM_SP_STORAGE, tfm_sst_set_req)
TFM_VENEER_FUNCTION(TFM_SP_STORAGE, tfm_sst_get_req)
TFM_VENEER_FUNCTION(TFM_SP_STORAGE, tfm_sst_get_info_req)
TFM_VENEER_FUNCTION(TFM_SP_STORAGE, tfm_sst_remove_req)
TFM_VENEER_FUNCTION(TFM_SP_STORAGE, tfm_sst_get_support_req)

/******** TFM_SP_AUDIT_LOG ********/
TFM_VENEER_FUNCTION(TFM_SP_AUDIT_LOG, audit_core_retrieve_record)
TFM_VENEER_FUNCTION(TFM_SP_AUDIT_LOG, audit_core_add_record)
TFM_VENEER_FUNCTION(TFM_SP_AUDIT_LOG, audit_core_get_info)
TFM_VENEER_FUNCTION(TFM_SP_AUDIT_LOG, audit_core_get_record_info)
TFM_VENEER_FUNCTION(TFM_SP_AUDIT_LOG, audit_core_delete_record)

/******** TFM_SP_CRYPTO ********/
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_import_key)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_destroy_key)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_get_key_information)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_export_key)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_key_policy_init)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_key_policy_set_usage)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_key_policy_get_usage)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_key_policy_get_algorithm)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_set_key_policy)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_get_key_policy)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_get_key_lifetime)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_set_key_lifetime)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_set_iv)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_encrypt_setup)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_decrypt_setup)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_update_wrapper)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_abort)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_cipher_finish)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_hash_setup)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_hash_update)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_hash_finish)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_hash_verify)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_hash_abort)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_sign_setup)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_verify_setup)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_update)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_sign_finish)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_verify_finish)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_mac_abort)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_aead_decrypt_wrapper)
TFM_VENEER_FUNCTION(TFM_SP_CRYPTO, tfm_crypto_aead_encrypt_wrapper)

/******** TFM_SP_PLATFORM ********/
TFM_VENEER_FUNCTION(TFM_SP_PLATFORM, platform_sp_system_reset)

/******** TFM_SP_INITIAL_ATTESTATION ********/
TFM_VENEER_FUNCTION(TFM_SP_INITIAL_ATTESTATION, initial_attest_get_token)
TFM_VENEER_FUNCTION(TFM_SP_INITIAL_ATTESTATION, initial_attest_get_token_size)

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST ********/
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST, spm_core_test_sfn)
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST, spm_core_test_sfn_init_success)
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST, spm_core_test_sfn_direct_recursion)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST_2 ********/
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST_2, spm_core_test_2_slave_service)
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST_2, spm_core_test_2_sfn_invert)
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST_2, spm_core_test_2_check_caller_client_id)
TFM_VENEER_FUNCTION(TFM_SP_CORE_TEST_2, spm_core_test_2_get_every_second_byte)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
/******** TFM_SP_SECURE_TEST_PARTITION ********/
TFM_VENEER_FUNCTION(TFM_SP_SECURE_TEST_PARTITION, tfm_secure_client_service_sfn_run_tests)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_SERVICE_TEST ********/
#endif /* TFM_PSA_API */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_CLIENT_TEST ********/
#endif /* TFM_PSA_API */

