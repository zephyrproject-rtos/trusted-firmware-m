#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_ipc_service STATIC)

target_sources(tfm_partition_ipc_service
    PRIVATE
        ./tfm_ipc_service_test.c
)

target_include_directories(tfm_partition_ipc_service
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_ipc_service
)

target_link_libraries(tfm_partition_ipc_service
    PRIVATE
        tfm_secure_api
        platform_s
        tfm_test_framework
        tfm_arch
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_partition_ipc_service
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_ipc_service
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_TEST_CORE_IPC
)
