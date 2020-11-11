#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_irq_test_1 STATIC)

target_sources(tfm_partition_irq_test_1
    PRIVATE
        ./tfm_irq_test_service_1.c
)

target_include_directories(tfm_partition_irq_test_1
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/test_services/tfm_irq_test_service_1
)

target_link_libraries(tfm_partition_irq_test_1
    PRIVATE
        tfm_secure_api
        psa_interface
        platform_s
        tfm_test_framework
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_partition_irq_test_1
)

target_include_directories(tfm_test_framework
    INTERFACE
        .
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_irq_test_1
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        $<$<BOOL:${TFM_IRQ_TEST}>:TFM_ENABLE_IRQ_TEST>
)
