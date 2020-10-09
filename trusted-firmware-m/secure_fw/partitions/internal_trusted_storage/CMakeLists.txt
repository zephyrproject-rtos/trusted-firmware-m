#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
    return()
endif()

cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_its STATIC)

target_include_directories(tfm_partition_its
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        # Required for ps_object_defs.h
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../protected_storage
)

target_sources(tfm_partition_its
    PRIVATE
        tfm_its_req_mngr.c
        tfm_internal_trusted_storage.c
        its_utils.c
        flash/its_flash.c
        flash/its_flash_nand.c
        flash/its_flash_nor.c
        flash/its_flash_ram.c
        flash/its_flash_info_internal.c
        flash/its_flash_info_external.c
        flash_fs/its_flash_fs.c
        flash_fs/its_flash_fs_dblock.c
        flash_fs/its_flash_fs_mblock.c
)

target_link_libraries(tfm_partition_its
    PRIVATE
        tfm_secure_api
        platform_s
        psa_interface
)

target_compile_definitions(tfm_partition_its
    PUBLIC
        $<$<BOOL:${PS_CREATE_FLASH_LAYOUT}>:PS_CREATE_FLASH_LAYOUT>
        $<$<BOOL:${PS_ENCRYPTION}>:PS_ENCRYPTION>
        $<$<BOOL:${PS_RAM_FS}>:PS_RAM_FS>
        $<$<BOOL:${PS_ROLLBACK_PROTECTION}>:PS_ROLLBACK_PROTECTION>
        $<$<BOOL:${PS_VALIDATE_METADATA_FROM_FLASH}>:PS_VALIDATE_METADATA_FROM_FLASH>
        $<$<BOOL:${PS_CRYPTO_AEAD_ALG}>:PS_CRYPTO_AEAD_ALG=${PS_CRYPTO_AEAD_ALG}>
    PRIVATE
        $<$<BOOL:${ITS_CREATE_FLASH_LAYOUT}>:ITS_CREATE_FLASH_LAYOUT>
        $<$<BOOL:${ITS_RAM_FS}>:ITS_RAM_FS>
        $<$<OR:$<BOOL:${ITS_VALIDATE_METADATA_FROM_FLASH}>,$<BOOL:PS_VALIDATE_METADATA_FROM_FLASH>>:ITS_VALIDATE_METADATA_FROM_FLASH>
        $<$<BOOL:${ITS_BUF_SIZE}>:ITS_BUF_SIZE>
)

############################ Secure API ########################################

target_sources(tfm_secure_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_its_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_its_secure_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_its
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
)
