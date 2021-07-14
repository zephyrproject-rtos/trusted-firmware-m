#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if (NOT TFM_PARTITION_INITIAL_ATTESTATION AND NOT FORWARD_PROT_MSG)
    return()
endif()

# This initial attestation test service provide a secure API to enable tests to
# fetch Initial Attestation public key.
# This test service shall be only enabled when the public key can only be
# fetched in runtime.
if (NOT ATTEST_TEST_GET_PUBLIC_KEY OR SYMMETRIC_INITIAL_ATTESTATION)
    return()
endif()

add_library(tfm_psa_rot_partition_attest_test_service STATIC)

target_sources(tfm_psa_rot_partition_attest_test_service
    PRIVATE
        ./tfm_attest_test_service.c
)

# The generated sources
target_sources(tfm_psa_rot_partition_attest_test_service
    PRIVATE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/test_services/tfm_attest_test_service/auto_generated/intermedia_tfm_attest_test_service.c>
)

target_sources(tfm_partitions
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/test_services/tfm_attest_test_service/auto_generated/load_info_tfm_attest_test_service.c>
)

# Set include directory
target_include_directories(tfm_psa_rot_partition_attest_test_service
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_attest_test_service
)

target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_attest_test_service
)

target_link_libraries(tfm_psa_rot_partition_attest_test_service
    PRIVATE
        tfm_secure_api
        psa_interface
        platform_s
        tfm_test_framework_s
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_psa_rot_partition_attest_test_service
)


# header files included by other libraries
add_library(tfm_psa_rot_partition_attest_test_service_defs INTERFACE)

target_include_directories(tfm_psa_rot_partition_attest_test_service_defs
    INTERFACE
        .
)

############################ NS API ############################################

target_sources(tfm_api_ns
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_attest_test_service_ns_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_attest_test_service_ns_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

target_include_directories(tfm_api_ns
    PUBLIC
        .
)

############################ Secure API ########################################

target_sources(tfm_secure_api
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_attest_test_service_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_attest_test_service_secure_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_attest_test_service
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        ATTEST_TEST_GET_PUBLIC_KEY
)
