#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

################################ t_cose defs ###################################

add_library(tfm_t_cose_defs INTERFACE)

target_include_directories(tfm_t_cose_defs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_definitions(tfm_t_cose_defs
    INTERFACE
        T_COSE_COMPILE_TIME_CONFIG
        T_COSE_USE_PSA_CRYPTO
        T_COSE_USE_PSA_CRYPTO_FROM_TFM
        T_COSE_DISABLE_CONTENT_TYPE
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_ES384>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_ES512>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_SIGN_VERIFY_TESTS>
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:T_COSE_DISABLE_SIGN1>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:T_COSE_DISABLE_MAC0>
        $<$<NOT:$<CONFIG:Debug>>:T_COSE_DISABLE_SHORT_CIRCUIT_SIGN>
)

############################### t_cose common ##################################

add_library(tfm_t_cose_common INTERFACE)

target_sources(tfm_t_cose_common
    INTERFACE
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_mac0_sign.c>
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_mac0_verify.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_sign1_sign.c>
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_sign1_verify.c>
        ${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_util.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/t_cose_parameters.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_adapters/t_cose_psa_crypto.c
)

############################ t_cose non secure #################################

add_library(tfm_t_cose_ns STATIC EXCLUDE_FROM_ALL)

target_link_libraries(tfm_t_cose_ns
    PUBLIC
        tfm_t_cose_common
        tfm_t_cose_defs
        tfm_qcbor
        tfm_ns_interface
)

############################ t_cose secure #####################################

add_library(tfm_t_cose_s STATIC EXCLUDE_FROM_ALL)

target_link_libraries(tfm_t_cose_s
    PUBLIC
        tfm_t_cose_common
        tfm_t_cose_defs
        tfm_qcbor
        psa_interface
)

############################ t_cose test #######################################

add_library(tfm_t_cose_test STATIC EXCLUDE_FROM_ALL)

target_sources(tfm_t_cose_test
    PRIVATE
        test/run_tests.c
        test/t_cose_make_psa_test_key.c
        test/t_cose_make_test_messages.c
        test/t_cose_sign_verify_test.c
        test/t_cose_test.c
)

target_include_directories(tfm_t_cose_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
)

target_link_libraries(tfm_t_cose_test
    PRIVATE
        tfm_t_cose_ns
)
