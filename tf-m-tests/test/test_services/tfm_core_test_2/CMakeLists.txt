#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_core_test_2 STATIC)

target_sources(tfm_partition_core_test_2
    PRIVATE
        tfm_ss_core_test_2.c
)

target_include_directories(tfm_partition_core_test_2
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_core_test_2
)

target_link_libraries(tfm_partition_core_test_2
    PRIVATE
        tfm_secure_api
        psa_interface
        platform_s
        tfm_partition_core_test
        tfm_arch
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_partition_core_test_2
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_core_test_2
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_TEST_CORE
)
