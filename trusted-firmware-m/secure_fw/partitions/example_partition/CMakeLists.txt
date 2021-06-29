#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_EXAMPLE)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

# The name of the target is required to be of the pattern
# tfm_app_rot_partition_x or tfm_psa_rot_partition_x, as it affects how the
# linker script will lay the partition in memory.
add_library(tfm_app_rot_partition_example STATIC)

target_include_directories(tfm_app_rot_partition_example
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/example_partition
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/example_partition
)

target_sources(tfm_app_rot_partition_example
    PRIVATE
        tfm_example_partition.c
)

# The generated sources
target_sources(tfm_app_rot_partition_example
    PRIVATE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/example_partition/auto_generated/intermedia_tfm_example_partition.c>
)
target_sources(tfm_partitions
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/example_partition/auto_generated/load_info_tfm_example_partition.c>
)

target_include_directories(tfm_app_rot_partition_example
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/example_partition
)

target_link_libraries(tfm_app_rot_partition_example
    PRIVATE
        tfm_secure_api
        psa_interface
        platform_s
        tfm_sprt
)

############################ NS API ########################################

target_sources(tfm_api_ns
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_example_partition_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_example_partition_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

target_include_directories(tfm_api_ns
    PUBLIC
        .
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_app_rot_partition_example
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_EXAMPLE
)
