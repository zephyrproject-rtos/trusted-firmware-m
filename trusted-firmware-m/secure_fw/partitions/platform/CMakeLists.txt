#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_PLATFORM)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_platform STATIC
    platform_sp.c
)

target_include_directories(tfm_partition_platform
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/platform
)

target_link_libraries(tfm_partition_platform
    PRIVATE
        platform_s
        psa_interface
        tfm_secure_api
        tfm_arch
        tfm_spm
)

############################ Secure API ########################################

target_sources(tfm_secure_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_platform_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_platform_secure_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_platform
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_PLATFORM
)
