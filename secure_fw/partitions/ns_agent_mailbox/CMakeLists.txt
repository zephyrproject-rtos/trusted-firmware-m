#-------------------------------------------------------------------------------
# Copyright (c) 2021-2024, Arm Limited. All rights reserved.
# Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_NS_AGENT_MAILBOX)
    return()
endif()

cmake_minimum_required(VERSION 3.21)

add_library(tfm_psa_rot_partition_ns_agent_mailbox STATIC)

add_dependencies(tfm_psa_rot_partition_ns_agent_mailbox manifest_tool)

target_sources(tfm_psa_rot_partition_ns_agent_mailbox
    PRIVATE
        $<$<NOT:$<BOOL:${TFM_PLAT_SPECIFIC_MULTI_CORE_COMM}>>:tfm_spe_mailbox.c>
        ns_agent_mailbox.c
        ns_agent_mailbox_rpc.c
        tfm_multi_core_client_id.c
        tfm_multi_core_mbox.c
)

target_link_libraries(tfm_psa_rot_partition_ns_agent_mailbox
    PRIVATE
        platform_common_interface
        tfm_spm
        tfm_sprt
        platform_s
)

# The generated sources
target_sources(tfm_psa_rot_partition_ns_agent_mailbox
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/ns_agent_mailbox/auto_generated/intermedia_ns_agent_mailbox.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/ns_agent_mailbox/auto_generated/load_info_ns_agent_mailbox.c
)

# Set include directory
target_include_directories(tfm_psa_rot_partition_ns_agent_mailbox
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/ns_agent_mailbox
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_ns_agent_mailbox
)

target_compile_definitions(tfm_config
    INTERFACE
        TFM_PARTITION_NS_AGENT_MAILBOX
)
