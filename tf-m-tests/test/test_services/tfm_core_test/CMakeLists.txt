#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_core_test STATIC)

target_sources(tfm_partition_core_test
    PRIVATE
        tfm_ss_core_test.c
)

target_include_directories(tfm_partition_core_test
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_core_test
)

target_link_libraries(tfm_partition_core_test
    PRIVATE
        tfm_secure_api
        psa_interface
        platform_s
        tfm_arch
        tfm_test_framework
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_partition_core_test
)

target_include_directories(tfm_test_framework
    INTERFACE
        .
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_core_test
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_TEST_CORE
)
