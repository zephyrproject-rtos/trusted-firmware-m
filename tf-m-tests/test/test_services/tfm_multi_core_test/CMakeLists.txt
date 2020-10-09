#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_policy(SET CMP0079 NEW)

if (NOT TFM_MULTI_CORE_TOPOLOGY)
    return()
endif()

add_library(tfm_partition_multi_core_test STATIC)

target_sources(tfm_partition_multi_core_test
    PRIVATE
        tfm_multi_core_test.c
)

target_include_directories(tfm_partition_multi_core_test
    PUBLIC
        .
)

target_link_libraries(tfm_partition_multi_core_test
    PRIVATE
        psa_interface
        tfm_secure_api
        platform_s
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_partition_multi_core_test
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_multi_core_test
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_TEST_MULTI_CORE
)
