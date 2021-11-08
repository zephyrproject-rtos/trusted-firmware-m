#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
# Copyright (c) 2021, Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_MULTI_CORE_TOPOLOGY)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_psa_rot_partition_ns_agent_mailbox STATIC)

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
