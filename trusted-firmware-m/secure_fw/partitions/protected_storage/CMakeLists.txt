#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_PROTECTED_STORAGE)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_ps STATIC)

target_include_directories(tfm_partition_ps
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/protected_storage
)

target_sources(tfm_partition_ps
    PRIVATE
        tfm_ps_req_mngr.c
        tfm_protected_storage.c
        ps_object_system.c
        ps_object_table.c
        ps_utils.c
        $<$<BOOL:${PS_ENCRYPTION}>:crypto/ps_crypto_interface.c>
        $<$<BOOL:${PS_ENCRYPTION}>:ps_encrypted_object.c>
        # The test_ps_nv_counters.c will be used instead, when secure test is ON
        # and PS_TEST_NV_COUNTERS ON
        $<$<NOT:$<AND:$<BOOL:${TEST_S}>,$<BOOL:${PS_TEST_NV_COUNTERS}>>>:nv_counters/ps_nv_counters.c>
)

target_link_libraries(tfm_partition_ps
    PRIVATE
        tfm_secure_api
        psa_interface
        secure_fw
        platform_s
        tfm_partition_its
)

############################ Secure API ########################################

target_sources(tfm_secure_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_ps_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_ps_secure_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_ps
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_PROTECTED_STORAGE
)
