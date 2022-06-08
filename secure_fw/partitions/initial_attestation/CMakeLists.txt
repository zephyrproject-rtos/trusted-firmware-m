#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_INITIAL_ATTESTATION)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_psa_rot_partition_attestation STATIC)

target_sources(tfm_psa_rot_partition_attestation PRIVATE
    tfm_attest.c
    tfm_attest_req_mngr.c
    attest_core.c
    attest_boot_data.c
    $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:attest_asymmetric_key.c>
    $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:attest_symmetric_key.c>
    attest_token_encode.c
)

# The generated sources
target_sources(tfm_psa_rot_partition_attestation
    PRIVATE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/initial_attestation/auto_generated/intermedia_tfm_initial_attestation.c>
)
target_sources(tfm_partitions
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/initial_attestation/auto_generated/load_info_tfm_initial_attestation.c>
)

# Set include directory
target_include_directories(tfm_psa_rot_partition_attestation
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/initial_attestation
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/initial_attestation
)

target_link_libraries(tfm_psa_rot_partition_attestation
    PRIVATE
        platform_s
        tfm_qcbor_s
        tfm_t_cose_s
        tfm_sprt
        tfm_boot_status
)

target_compile_definitions(tfm_psa_rot_partition_attestation
    PRIVATE
        $<$<BOOL:${ATTEST_INCLUDE_TEST_CODE}>:INCLUDE_TEST_CODE>
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:SYMMETRIC_INITIAL_ATTESTATION>
        $<$<BOOL:${ATTEST_INCLUDE_OPTIONAL_CLAIMS}>:INCLUDE_OPTIONAL_CLAIMS>
        $<$<BOOL:${ATTEST_INCLUDE_COSE_KEY_ID}>:INCLUDE_COSE_KEY_ID>
        $<$<NOT:$<BOOL:${PLATFORM_DEFAULT_ATTEST_HAL}>>:CLAIM_VALUE_CHECK>
        ATTEST_TOKEN_PROFILE_${ATTEST_TOKEN_PROFILE}
        $<$<NOT:$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>>:ATTEST_KEY_BITS=${ATTEST_KEY_BITS}>
)

########################### Attest defs ########################################

add_library(tfm_attestation_defs INTERFACE)

target_include_directories(tfm_attestation_defs
    INTERFACE
        .
)

target_compile_definitions(tfm_attestation_defs
    INTERFACE
        ATTEST_TOKEN_PROFILE_${ATTEST_TOKEN_PROFILE}
)

############################ Secure API ########################################

target_sources(tfm_sprt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_attest_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_attest_secure_api.c
    PROPERTIES
        COMPILE_FLAGS
            $<$<C_COMPILER_ID:ARMClang>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:GNU>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:IAR>:>
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_attestation
)


target_compile_definitions(tfm_partition_defs
    INTERFACE
    TFM_PARTITION_INITIAL_ATTESTATION
)

################ Display the configuration being applied #######################

include(utils)
dump_options("Initial Attestation config"
"
    ATTEST_STACK_SIZE
"
)
