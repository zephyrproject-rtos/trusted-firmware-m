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

target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/load_info_ns_agent_mailbox.c
)
