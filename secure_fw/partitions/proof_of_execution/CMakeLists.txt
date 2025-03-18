#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.21)

add_library(tfm_app_rot_partition_pox STATIC)

add_dependencies(tfm_app_rot_partition_pox manifest_tool)

target_sources(tfm_app_rot_partition_pox
    PRIVATE
        pox.c
        pox_execute.c
        pox_handler.c
        pox_report.c
        pox_token_encode.c
)

# The generated sources
target_sources(tfm_app_rot_partition_pox
PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/proof_of_execution/auto_generated/intermedia_tfm_pox.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/proof_of_execution/auto_generated/load_info_tfm_pox.c
)

# Set include directory
target_include_directories(tfm_app_rot_partition_pox
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/proof_of_execution
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/proof_of_execution
)

target_link_libraries(tfm_app_rot_partition_pox
    PRIVATE
        platform_s
        tfm_config
        tfm_t_cose_s
        tfm_sprt
        tfm_boot_status
        qcbor
        psa_crypto_config
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_app_rot_partition_pox
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_app_rot_partition_pox
)

target_compile_definitions(tfm_config
    INTERFACE
        TFM_PARTITION_POX
)
