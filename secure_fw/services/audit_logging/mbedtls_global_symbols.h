/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MBEDTLS_GLOBAL_SYMBOLS_H__
#define __MBEDTLS_GLOBAL_SYMBOLS_H__

#ifndef LIB_PREFIX_NAME
#warning "LIB_PREFIX_NAME is undefined!!!"
#define LIB_PREFIX_NAME
#endif

#define _CONCAT(A,B) A##B
#define CONCAT(A,B) _CONCAT(A,B)

#define mbedtls_aes_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_crypt_cbc)
#define mbedtls_aes_crypt_cfb128 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_crypt_cfb128)
#define mbedtls_aes_crypt_cfb8 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_crypt_cfb8)
#define mbedtls_aes_crypt_ctr \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_crypt_ctr)
#define mbedtls_aes_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_crypt_ecb)
#define mbedtls_aes_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_decrypt)
#define mbedtls_aes_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_encrypt)
#define mbedtls_aes_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_free)
#define mbedtls_aes_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_init)
#define mbedtls_aes_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_self_test)
#define mbedtls_aes_setkey_dec \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_setkey_dec)
#define mbedtls_aes_setkey_enc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_aes_setkey_enc)
#define mbedtls_arc4_crypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_arc4_crypt)
#define mbedtls_arc4_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_arc4_free)
#define mbedtls_arc4_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_arc4_init)
#define mbedtls_arc4_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_arc4_self_test)
#define mbedtls_arc4_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_arc4_setup)
#define mbedtls_asn1_find_named_data \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_find_named_data)
#define mbedtls_asn1_free_named_data \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_free_named_data)
#define mbedtls_asn1_free_named_data_list \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_free_named_data_list)
#define mbedtls_asn1_get_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_alg)
#define mbedtls_asn1_get_alg_null \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_alg_null)
#define mbedtls_asn1_get_bitstring \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_bitstring)
#define mbedtls_asn1_get_bitstring_null \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_bitstring_null)
#define mbedtls_asn1_get_bool \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_bool)
#define mbedtls_asn1_get_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_int)
#define mbedtls_asn1_get_len \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_len)
#define mbedtls_asn1_get_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_mpi)
#define mbedtls_asn1_get_sequence_of \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_sequence_of)
#define mbedtls_asn1_get_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_get_tag)
#define mbedtls_asn1_store_named_data \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_store_named_data)
#define mbedtls_asn1_write_algorithm_identifier \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_algorithm_identifier)
#define mbedtls_asn1_write_bitstring \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_bitstring)
#define mbedtls_asn1_write_bool \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_bool)
#define mbedtls_asn1_write_ia5_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_ia5_string)
#define mbedtls_asn1_write_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_int)
#define mbedtls_asn1_write_len \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_len)
#define mbedtls_asn1_write_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_mpi)
#define mbedtls_asn1_write_null \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_null)
#define mbedtls_asn1_write_octet_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_octet_string)
#define mbedtls_asn1_write_oid \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_oid)
#define mbedtls_asn1_write_printable_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_printable_string)
#define mbedtls_asn1_write_raw_buffer \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_raw_buffer)
#define mbedtls_asn1_write_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_asn1_write_tag)
#define mbedtls_base64_decode \
        CONCAT(LIB_PREFIX_NAME, mbedtls_base64_decode)
#define mbedtls_base64_encode \
        CONCAT(LIB_PREFIX_NAME, mbedtls_base64_encode)
#define mbedtls_base64_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_base64_self_test)
#define mbedtls_blowfish_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_crypt_cbc)
#define mbedtls_blowfish_crypt_cfb64 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_crypt_cfb64)
#define mbedtls_blowfish_crypt_ctr \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_crypt_ctr)
#define mbedtls_blowfish_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_crypt_ecb)
#define mbedtls_blowfish_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_free)
#define mbedtls_blowfish_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_init)
#define mbedtls_blowfish_setkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_blowfish_setkey)
#define mbedtls_calloc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_calloc)
#define mbedtls_camellia_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_crypt_cbc)
#define mbedtls_camellia_crypt_cfb128 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_crypt_cfb128)
#define mbedtls_camellia_crypt_ctr \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_crypt_ctr)
#define mbedtls_camellia_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_crypt_ecb)
#define mbedtls_camellia_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_free)
#define mbedtls_camellia_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_init)
#define mbedtls_camellia_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_self_test)
#define mbedtls_camellia_setkey_dec \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_setkey_dec)
#define mbedtls_camellia_setkey_enc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_camellia_setkey_enc)
#define mbedtls_ccm_auth_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_auth_decrypt)
#define mbedtls_ccm_encrypt_and_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_encrypt_and_tag)
#define mbedtls_ccm_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_free)
#define mbedtls_ccm_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_init)
#define mbedtls_ccm_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_self_test)
#define mbedtls_ccm_setkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ccm_setkey)
#define mbedtls_cipher_auth_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_auth_decrypt)
#define mbedtls_cipher_auth_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_auth_encrypt)
#define mbedtls_cipher_check_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_check_tag)
#define mbedtls_cipher_crypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_crypt)
#define mbedtls_cipher_definitions \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_definitions)
#define mbedtls_cipher_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_finish)
#define mbedtls_cipher_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_free)
#define mbedtls_cipher_info_from_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_info_from_string)
#define mbedtls_cipher_info_from_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_info_from_type)
#define mbedtls_cipher_info_from_values \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_info_from_values)
#define mbedtls_cipher_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_init)
#define mbedtls_cipher_list \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_list)
#define mbedtls_cipher_reset \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_reset)
#define mbedtls_cipher_set_iv \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_set_iv)
#define mbedtls_cipher_setkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_setkey)
#define mbedtls_cipher_set_padding_mode \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_set_padding_mode)
#define mbedtls_cipher_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_setup)
#define mbedtls_cipher_supported \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_supported)
#define mbedtls_cipher_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_update)
#define mbedtls_cipher_update_ad \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_update_ad)
#define mbedtls_cipher_write_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_cipher_write_tag)
#define mbedtls_ctr_drbg_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_free)
#define mbedtls_ctr_drbg_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_init)
#define mbedtls_ctr_drbg_random \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_random)
#define mbedtls_ctr_drbg_random_with_add \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_random_with_add)
#define mbedtls_ctr_drbg_reseed \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_reseed)
#define mbedtls_ctr_drbg_seed \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_seed)
#define mbedtls_ctr_drbg_seed_entropy_len \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_seed_entropy_len)
#define mbedtls_ctr_drbg_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_self_test)
#define mbedtls_ctr_drbg_set_entropy_len \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_set_entropy_len)
#define mbedtls_ctr_drbg_set_prediction_resistance \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_set_prediction_resistance)
#define mbedtls_ctr_drbg_set_reseed_interval \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_set_reseed_interval)
#define mbedtls_ctr_drbg_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ctr_drbg_update)
#define mbedtls_des3_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_crypt_cbc)
#define mbedtls_des3_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_crypt_ecb)
#define mbedtls_des3_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_free)
#define mbedtls_des3_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_init)
#define mbedtls_des3_set2key_dec \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_set2key_dec)
#define mbedtls_des3_set2key_enc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_set2key_enc)
#define mbedtls_des3_set3key_dec \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_set3key_dec)
#define mbedtls_des3_set3key_enc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des3_set3key_enc)
#define mbedtls_des_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_crypt_cbc)
#define mbedtls_des_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_crypt_ecb)
#define mbedtls_des_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_free)
#define mbedtls_des_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_init)
#define mbedtls_des_key_check_key_parity \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_key_check_key_parity)
#define mbedtls_des_key_check_weak \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_key_check_weak)
#define mbedtls_des_key_set_parity \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_key_set_parity)
#define mbedtls_des_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_self_test)
#define mbedtls_des_setkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_setkey)
#define mbedtls_des_setkey_dec \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_setkey_dec)
#define mbedtls_des_setkey_enc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_des_setkey_enc)
#define mbedtls_dhm_calc_secret \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_calc_secret)
#define mbedtls_dhm_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_free)
#define mbedtls_dhm_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_init)
#define mbedtls_dhm_make_params \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_make_params)
#define mbedtls_dhm_make_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_make_public)
#define mbedtls_dhm_parse_dhm \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_parse_dhm)
#define mbedtls_dhm_read_params \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_read_params)
#define mbedtls_dhm_read_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_read_public)
#define mbedtls_dhm_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_dhm_self_test)
#define mbedtls_ecdh_calc_secret \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_calc_secret)
#define mbedtls_ecdh_compute_shared \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_compute_shared)
#define mbedtls_ecdh_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_free)
#define mbedtls_ecdh_gen_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_gen_public)
#define mbedtls_ecdh_get_params \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_get_params)
#define mbedtls_ecdh_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_init)
#define mbedtls_ecdh_make_params \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_make_params)
#define mbedtls_ecdh_make_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_make_public)
#define mbedtls_ecdh_read_params \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_read_params)
#define mbedtls_ecdh_read_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdh_read_public)
#define mbedtls_ecdsa_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_free)
#define mbedtls_ecdsa_from_keypair \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_from_keypair)
#define mbedtls_ecdsa_genkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_genkey)
#define mbedtls_ecdsa_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_info)
#define mbedtls_ecdsa_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_init)
#define mbedtls_ecdsa_read_signature \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_read_signature)
#define mbedtls_ecdsa_sign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_sign)
#define mbedtls_ecdsa_sign_det \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_sign_det)
#define mbedtls_ecdsa_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_verify)
#define mbedtls_ecdsa_write_signature \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_write_signature)
#define mbedtls_ecdsa_write_signature_det \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecdsa_write_signature_det)
#define mbedtls_eckeydh_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_eckeydh_info)
#define mbedtls_eckey_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_eckey_info)
#define mbedtls_ecp_check_privkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_check_privkey)
#define mbedtls_ecp_check_pubkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_check_pubkey)
#define mbedtls_ecp_check_pub_priv \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_check_pub_priv)
#define mbedtls_ecp_copy \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_copy)
#define mbedtls_ecp_curve_info_from_grp_id \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_curve_info_from_grp_id)
#define mbedtls_ecp_curve_info_from_name \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_curve_info_from_name)
#define mbedtls_ecp_curve_info_from_tls_id \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_curve_info_from_tls_id)
#define mbedtls_ecp_curve_list \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_curve_list)
#define mbedtls_ecp_gen_key \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_gen_key)
#define mbedtls_ecp_gen_keypair \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_gen_keypair)
#define mbedtls_ecp_gen_keypair_base \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_gen_keypair_base)
#define mbedtls_ecp_group_copy \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_group_copy)
#define mbedtls_ecp_group_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_group_free)
#define mbedtls_ecp_group_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_group_init)
#define mbedtls_ecp_group_load \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_group_load)
#define mbedtls_ecp_grp_id_list \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_grp_id_list)
#define mbedtls_ecp_is_zero \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_is_zero)
#define mbedtls_ecp_keypair_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_keypair_free)
#define mbedtls_ecp_keypair_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_keypair_init)
#define mbedtls_ecp_mul \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_mul)
#define mbedtls_ecp_muladd \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_muladd)
#define mbedtls_ecp_point_cmp \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_cmp)
#define mbedtls_ecp_point_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_free)
#define mbedtls_ecp_point_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_init)
#define mbedtls_ecp_point_read_binary \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_read_binary)
#define mbedtls_ecp_point_read_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_read_string)
#define mbedtls_ecp_point_write_binary \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_point_write_binary)
#define mbedtls_ecp_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_self_test)
#define mbedtls_ecp_set_zero \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_set_zero)
#define mbedtls_ecp_tls_read_group \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_tls_read_group)
#define mbedtls_ecp_tls_read_point \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_tls_read_point)
#define mbedtls_ecp_tls_write_group \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_tls_write_group)
#define mbedtls_ecp_tls_write_point \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ecp_tls_write_point)
#define mbedtls_entropy_add_source \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_add_source)
#define mbedtls_entropy_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_free)
#define mbedtls_entropy_func \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_func)
#define mbedtls_entropy_gather \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_gather)
#define mbedtls_entropy_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_init)
#define mbedtls_entropy_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_self_test)
#define mbedtls_entropy_update_manual \
        CONCAT(LIB_PREFIX_NAME, mbedtls_entropy_update_manual)
#define mbedtls_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_free)
#define mbedtls_gcm_auth_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_auth_decrypt)
#define mbedtls_gcm_crypt_and_tag \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_crypt_and_tag)
#define mbedtls_gcm_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_finish)
#define mbedtls_gcm_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_free)
#define mbedtls_gcm_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_init)
#define mbedtls_gcm_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_self_test)
#define mbedtls_gcm_setkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_setkey)
#define mbedtls_gcm_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_starts)
#define mbedtls_gcm_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_gcm_update)
#define mbedtls_hmac_drbg_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_free)
#define mbedtls_hmac_drbg_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_init)
#define mbedtls_hmac_drbg_random \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_random)
#define mbedtls_hmac_drbg_random_with_add \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_random_with_add)
#define mbedtls_hmac_drbg_reseed \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_reseed)
#define mbedtls_hmac_drbg_seed \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_seed)
#define mbedtls_hmac_drbg_seed_buf \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_seed_buf)
#define mbedtls_hmac_drbg_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_self_test)
#define mbedtls_hmac_drbg_set_entropy_len \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_set_entropy_len)
#define mbedtls_hmac_drbg_set_prediction_resistance \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_set_prediction_resistance)
#define mbedtls_hmac_drbg_set_reseed_interval \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_set_reseed_interval)
#define mbedtls_hmac_drbg_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_hmac_drbg_update)
#define mbedtls_internal_aes_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_internal_aes_decrypt)
#define mbedtls_internal_aes_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_internal_aes_encrypt)
#define mbedtls_md \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md)
#define mbedtls_md5 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5)
#define mbedtls_md5_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_clone)
#define mbedtls_md5_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_finish)
#define mbedtls_md5_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_free)
#define mbedtls_md5_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_info)
#define mbedtls_md5_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_init)
#define mbedtls_md5_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_process)
#define mbedtls_md5_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_self_test)
#define mbedtls_md5_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_starts)
#define mbedtls_md5_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md5_update)
#define mbedtls_md_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_clone)
#define mbedtls_md_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_finish)
#define mbedtls_md_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_free)
#define mbedtls_md_get_name \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_get_name)
#define mbedtls_md_get_size \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_get_size)
#define mbedtls_md_get_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_get_type)
#define mbedtls_md_hmac \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_hmac)
#define mbedtls_md_hmac_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_hmac_finish)
#define mbedtls_md_hmac_reset \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_hmac_reset)
#define mbedtls_md_hmac_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_hmac_starts)
#define mbedtls_md_hmac_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_hmac_update)
#define mbedtls_md_info_from_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_info_from_string)
#define mbedtls_md_info_from_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_info_from_type)
#define mbedtls_md_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_init)
#define mbedtls_md_init_ctx \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_init_ctx)
#define mbedtls_md_list \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_list)
#define mbedtls_md_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_process)
#define mbedtls_md_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_setup)
#define mbedtls_md_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_starts)
#define mbedtls_md_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_md_update)
#define mbedtls_memory_buffer_alloc_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_memory_buffer_alloc_free)
#define mbedtls_memory_buffer_alloc_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_memory_buffer_alloc_init)
#define mbedtls_memory_buffer_alloc_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_memory_buffer_alloc_self_test)
#define mbedtls_memory_buffer_alloc_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_memory_buffer_alloc_verify)
#define mbedtls_memory_buffer_set_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_memory_buffer_set_verify)
#define mbedtls_mpi_add_abs \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_add_abs)
#define mbedtls_mpi_add_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_add_int)
#define mbedtls_mpi_add_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_add_mpi)
#define mbedtls_mpi_bitlen \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_bitlen)
#define mbedtls_mpi_cmp_abs \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_cmp_abs)
#define mbedtls_mpi_cmp_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_cmp_int)
#define mbedtls_mpi_cmp_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_cmp_mpi)
#define mbedtls_mpi_copy \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_copy)
#define mbedtls_mpi_div_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_div_int)
#define mbedtls_mpi_div_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_div_mpi)
#define mbedtls_mpi_exp_mod \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_exp_mod)
#define mbedtls_mpi_fill_random \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_fill_random)
#define mbedtls_mpi_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_free)
#define mbedtls_mpi_gcd \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_gcd)
#define mbedtls_mpi_gen_prime \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_gen_prime)
#define mbedtls_mpi_get_bit \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_get_bit)
#define mbedtls_mpi_grow \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_grow)
#define mbedtls_mpi_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_init)
#define mbedtls_mpi_inv_mod \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_inv_mod)
#define mbedtls_mpi_is_prime \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_is_prime)
#define mbedtls_mpi_lsb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_lsb)
#define mbedtls_mpi_lset \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_lset)
#define mbedtls_mpi_mod_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_mod_int)
#define mbedtls_mpi_mod_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_mod_mpi)
#define mbedtls_mpi_mul_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_mul_int)
#define mbedtls_mpi_mul_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_mul_mpi)
#define mbedtls_mpi_read_binary \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_read_binary)
#define mbedtls_mpi_read_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_read_string)
#define mbedtls_mpi_safe_cond_assign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_safe_cond_assign)
#define mbedtls_mpi_safe_cond_swap \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_safe_cond_swap)
#define mbedtls_mpi_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_self_test)
#define mbedtls_mpi_set_bit \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_set_bit)
#define mbedtls_mpi_shift_l \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_shift_l)
#define mbedtls_mpi_shift_r \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_shift_r)
#define mbedtls_mpi_shrink \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_shrink)
#define mbedtls_mpi_size \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_size)
#define mbedtls_mpi_sub_abs \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_sub_abs)
#define mbedtls_mpi_sub_int \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_sub_int)
#define mbedtls_mpi_sub_mpi \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_sub_mpi)
#define mbedtls_mpi_swap \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_swap)
#define mbedtls_mpi_write_binary \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_write_binary)
#define mbedtls_mpi_write_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_mpi_write_string)
#define mbedtls_oid_get_attr_short_name \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_attr_short_name)
#define mbedtls_oid_get_cipher_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_cipher_alg)
#define mbedtls_oid_get_ec_grp \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_ec_grp)
#define mbedtls_oid_get_extended_key_usage \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_extended_key_usage)
#define mbedtls_oid_get_md_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_md_alg)
#define mbedtls_oid_get_numeric_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_numeric_string)
#define mbedtls_oid_get_oid_by_ec_grp \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_oid_by_ec_grp)
#define mbedtls_oid_get_oid_by_md \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_oid_by_md)
#define mbedtls_oid_get_oid_by_pk_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_oid_by_pk_alg)
#define mbedtls_oid_get_oid_by_sig_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_oid_by_sig_alg)
#define mbedtls_oid_get_pk_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_pk_alg)
#define mbedtls_oid_get_pkcs12_pbe_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_pkcs12_pbe_alg)
#define mbedtls_oid_get_sig_alg \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_sig_alg)
#define mbedtls_oid_get_sig_alg_desc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_sig_alg_desc)
#define mbedtls_oid_get_x509_ext_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_oid_get_x509_ext_type)
#define mbedtls_pem_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pem_free)
#define mbedtls_pem_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pem_init)
#define mbedtls_pem_read_buffer \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pem_read_buffer)
#define mbedtls_pem_write_buffer \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pem_write_buffer)
#define mbedtls_pk_can_do \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_can_do)
#define mbedtls_pk_check_pair \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_check_pair)
#define mbedtls_pkcs12_derivation \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs12_derivation)
#define mbedtls_pkcs12_pbe \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs12_pbe)
#define mbedtls_pkcs12_pbe_sha1_rc4_128 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs12_pbe_sha1_rc4_128)
#define mbedtls_pkcs5_pbes2 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs5_pbes2)
#define mbedtls_pkcs5_pbkdf2_hmac \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs5_pbkdf2_hmac)
#define mbedtls_pkcs5_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pkcs5_self_test)
#define mbedtls_pk_debug \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_debug)
#define mbedtls_pk_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_decrypt)
#define mbedtls_pk_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_encrypt)
#define mbedtls_pk_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_free)
#define mbedtls_pk_get_bitlen \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_get_bitlen)
#define mbedtls_pk_get_name \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_get_name)
#define mbedtls_pk_get_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_get_type)
#define mbedtls_pk_info_from_type \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_info_from_type)
#define mbedtls_pk_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_init)
#define mbedtls_pk_parse_key \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_parse_key)
#define mbedtls_pk_parse_public_key \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_parse_public_key)
#define mbedtls_pk_parse_subpubkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_parse_subpubkey)
#define mbedtls_pk_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_setup)
#define mbedtls_pk_setup_rsa_alt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_setup_rsa_alt)
#define mbedtls_pk_sign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_sign)
#define mbedtls_pk_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_verify)
#define mbedtls_pk_verify_ext \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_verify_ext)
#define mbedtls_pk_write_key_der \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_write_key_der)
#define mbedtls_pk_write_key_pem \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_write_key_pem)
#define mbedtls_pk_write_pubkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_write_pubkey)
#define mbedtls_pk_write_pubkey_der \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_write_pubkey_der)
#define mbedtls_pk_write_pubkey_pem \
        CONCAT(LIB_PREFIX_NAME, mbedtls_pk_write_pubkey_pem)
#define mbedtls_platform_set_calloc_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_platform_set_calloc_free)
#define mbedtls_platform_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_platform_setup)
#define mbedtls_platform_teardown \
        CONCAT(LIB_PREFIX_NAME, mbedtls_platform_teardown)
#define mbedtls_ripemd160 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160)
#define mbedtls_ripemd160_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_clone)
#define mbedtls_ripemd160_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_finish)
#define mbedtls_ripemd160_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_free)
#define mbedtls_ripemd160_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_info)
#define mbedtls_ripemd160_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_init)
#define mbedtls_ripemd160_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_process)
#define mbedtls_ripemd160_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_self_test)
#define mbedtls_ripemd160_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_starts)
#define mbedtls_ripemd160_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_ripemd160_update)
#define mbedtls_rsa_alt_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_alt_info)
#define mbedtls_rsa_check_privkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_check_privkey)
#define mbedtls_rsa_check_pubkey \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_check_pubkey)
#define mbedtls_rsa_check_pub_priv \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_check_pub_priv)
#define mbedtls_rsa_copy \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_copy)
#define mbedtls_rsa_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_free)
#define mbedtls_rsa_gen_key \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_gen_key)
#define mbedtls_rsa_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_info)
#define mbedtls_rsa_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_init)
#define mbedtls_rsa_pkcs1_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_pkcs1_decrypt)
#define mbedtls_rsa_pkcs1_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_pkcs1_encrypt)
#define mbedtls_rsa_pkcs1_sign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_pkcs1_sign)
#define mbedtls_rsa_pkcs1_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_pkcs1_verify)
#define mbedtls_rsa_private \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_private)
#define mbedtls_rsa_public \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_public)
#define mbedtls_rsa_rsaes_oaep_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsaes_oaep_decrypt)
#define mbedtls_rsa_rsaes_oaep_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsaes_oaep_encrypt)
#define mbedtls_rsa_rsaes_pkcs1_v15_decrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsaes_pkcs1_v15_decrypt)
#define mbedtls_rsa_rsaes_pkcs1_v15_encrypt \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsaes_pkcs1_v15_encrypt)
#define mbedtls_rsa_rsassa_pkcs1_v15_sign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsassa_pkcs1_v15_sign)
#define mbedtls_rsa_rsassa_pkcs1_v15_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsassa_pkcs1_v15_verify)
#define mbedtls_rsa_rsassa_pss_sign \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsassa_pss_sign)
#define mbedtls_rsa_rsassa_pss_verify \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsassa_pss_verify)
#define mbedtls_rsa_rsassa_pss_verify_ext \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_rsassa_pss_verify_ext)
#define mbedtls_rsa_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_self_test)
#define mbedtls_rsa_set_padding \
        CONCAT(LIB_PREFIX_NAME, mbedtls_rsa_set_padding)
#define mbedtls_sha1 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1)
#define mbedtls_sha1_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_clone)
#define mbedtls_sha1_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_finish)
#define mbedtls_sha1_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_free)
#define mbedtls_sha1_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_info)
#define mbedtls_sha1_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_init)
#define mbedtls_sha1_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_process)
#define mbedtls_sha1_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_self_test)
#define mbedtls_sha1_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_starts)
#define mbedtls_sha1_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha1_update)
#define mbedtls_sha224_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha224_info)
#define mbedtls_sha256 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256)
#define mbedtls_sha256_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_clone)
#define mbedtls_sha256_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_finish)
#define mbedtls_sha256_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_free)
#define mbedtls_sha256_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_info)
#define mbedtls_sha256_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_init)
#define mbedtls_sha256_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_process)
#define mbedtls_sha256_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_self_test)
#define mbedtls_sha256_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_starts)
#define mbedtls_sha256_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha256_update)
#define mbedtls_sha384_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha384_info)
#define mbedtls_sha512 \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512)
#define mbedtls_sha512_clone \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_clone)
#define mbedtls_sha512_finish \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_finish)
#define mbedtls_sha512_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_free)
#define mbedtls_sha512_info \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_info)
#define mbedtls_sha512_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_init)
#define mbedtls_sha512_process \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_process)
#define mbedtls_sha512_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_self_test)
#define mbedtls_sha512_starts \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_starts)
#define mbedtls_sha512_update \
        CONCAT(LIB_PREFIX_NAME, mbedtls_sha512_update)
#define mbedtls_strerror \
        CONCAT(LIB_PREFIX_NAME, mbedtls_strerror)
#define mbedtls_version_check_feature \
        CONCAT(LIB_PREFIX_NAME, mbedtls_version_check_feature)
#define mbedtls_version_get_number \
        CONCAT(LIB_PREFIX_NAME, mbedtls_version_get_number)
#define mbedtls_version_get_string \
        CONCAT(LIB_PREFIX_NAME, mbedtls_version_get_string)
#define mbedtls_version_get_string_full \
        CONCAT(LIB_PREFIX_NAME, mbedtls_version_get_string_full)
#define mbedtls_xtea_crypt_cbc \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_crypt_cbc)
#define mbedtls_xtea_crypt_ecb \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_crypt_ecb)
#define mbedtls_xtea_free \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_free)
#define mbedtls_xtea_init \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_init)
#define mbedtls_xtea_self_test \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_self_test)
#define mbedtls_xtea_setup \
        CONCAT(LIB_PREFIX_NAME, mbedtls_xtea_setup)

#endif /* __MBEDTLS_GLOBAL_SYMBOLS_H__ */
