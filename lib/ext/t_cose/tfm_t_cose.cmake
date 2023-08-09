#-------------------------------------------------------------------------------
# Copyright (c) 2020-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

################################ t_cose defs ###################################

add_library(tfm_t_cose_defs INTERFACE)

target_include_directories(tfm_t_cose_defs
    INTERFACE
        $<BUILD_INTERFACE:${T_COSE_PATH}/inc>
        $<BUILD_INTERFACE:${T_COSE_PATH}/src>
)

target_compile_definitions(tfm_t_cose_defs
    INTERFACE
        T_COSE_USE_PSA_CRYPTO
        T_COSE_DISABLE_CONTENT_TYPE
        T_COSE_DISABLE_COSE_SIGN
        T_COSE_DISABLE_KEYWRAP
        T_COSE_DISABLE_PS256
        T_COSE_DISABLE_PS384
        T_COSE_DISABLE_PS512
        T_COSE_DISABLE_SHORT_CIRCUIT_SIGN
        $<$<OR:$<NOT:$<STREQUAL:${ATTEST_KEY_BITS},384>>,$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_ES384>
        $<$<OR:$<NOT:$<STREQUAL:${ATTEST_KEY_BITS},521>>,$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_ES512>
)

############################### t_cose common ##################################

add_library(tfm_t_cose_common INTERFACE)

target_sources(tfm_t_cose_common
    INTERFACE
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:${T_COSE_PATH}/src/t_cose_mac_compute.c>
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:${T_COSE_PATH}/src/t_cose_mac_validate.c>

        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_sign_sign.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_sign1_sign.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_signature_sign_main.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_sign_verify.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_sign1_verify.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${T_COSE_PATH}/src/t_cose_signature_verify_main.c>

        ${T_COSE_PATH}/crypto_adapters/t_cose_psa_crypto.c
        ${T_COSE_PATH}/src/t_cose_key.c
        ${T_COSE_PATH}/src/t_cose_parameters.c
        ${T_COSE_PATH}/src/t_cose_util.c
)

target_link_libraries(tfm_t_cose_common
    INTERFACE
        tfm_config
        psa_crypto_config
)
