#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_psa_rot_partition_its STATIC)

target_include_directories(tfm_psa_rot_partition_its
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/internal_trusted_storage
    PUBLIC
        # Required for ps_object_defs.h
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../protected_storage
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/internal_trusted_storage
)

target_sources(tfm_psa_rot_partition_its
    PRIVATE
        tfm_its_req_mngr.c
        tfm_internal_trusted_storage.c
        its_utils.c
        flash/its_flash.c
        flash/its_flash_nand.c
        flash/its_flash_nor.c
        flash/its_flash_ram.c
        flash_fs/its_flash_fs.c
        flash_fs/its_flash_fs_dblock.c
        flash_fs/its_flash_fs_mblock.c
)

# The generated sources
target_sources(tfm_psa_rot_partition_its
    PRIVATE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/internal_trusted_storage/auto_generated/intermedia_tfm_internal_trusted_storage.c>
)
target_sources(tfm_partitions
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:
            ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/internal_trusted_storage/auto_generated/load_info_tfm_internal_trusted_storage.c>
)

target_link_libraries(tfm_psa_rot_partition_its
    PRIVATE
        platform_s
        tfm_sprt
)

target_compile_definitions(tfm_psa_rot_partition_its
    PUBLIC
        $<$<BOOL:${PS_CREATE_FLASH_LAYOUT}>:PS_CREATE_FLASH_LAYOUT>
        $<$<BOOL:${PS_ENCRYPTION}>:PS_ENCRYPTION>
        $<$<BOOL:${PS_RAM_FS}>:PS_RAM_FS>
        $<$<BOOL:${PS_ROLLBACK_PROTECTION}>:PS_ROLLBACK_PROTECTION>
        $<$<BOOL:${PS_VALIDATE_METADATA_FROM_FLASH}>:PS_VALIDATE_METADATA_FROM_FLASH>
        PS_MAX_ASSET_SIZE=${PS_MAX_ASSET_SIZE}
        PS_NUM_ASSETS=${PS_NUM_ASSETS}
        PS_CRYPTO_AEAD_ALG=${PS_CRYPTO_AEAD_ALG}
    PRIVATE
        $<$<BOOL:${ITS_CREATE_FLASH_LAYOUT}>:ITS_CREATE_FLASH_LAYOUT>
        $<$<BOOL:${ITS_RAM_FS}>:ITS_RAM_FS>
        $<$<OR:$<BOOL:${ITS_VALIDATE_METADATA_FROM_FLASH}>,$<BOOL:${PS_VALIDATE_METADATA_FROM_FLASH}>>:ITS_VALIDATE_METADATA_FROM_FLASH>
        ITS_MAX_ASSET_SIZE=${ITS_MAX_ASSET_SIZE}
        ITS_NUM_ASSETS=${ITS_NUM_ASSETS}
        $<$<BOOL:${ITS_BUF_SIZE}>:ITS_BUF_SIZE=${ITS_BUF_SIZE}>
)

################ Display the configuration being applied #######################

include(utils)
    dump_options("Storage config"
    "
        PS_CREATE_FLASH_LAYOUT;
        PS_ENCRYPTION;
        PS_RAM_FS;
        PS_ROLLBACK_PROTECTION;
        PS_VALIDATE_METADATA_FROM_FLASH;
        PS_MAX_ASSET_SIZE;
        PS_NUM_ASSETS;
        PS_CRYPTO_AEAD_ALG;
        PS_STACK_SIZE;

        ITS_CREATE_FLASH_LAYOUT;
        ITS_RAM_FS;
        ITS_VALIDATE_METADATA_FROM_FLASH;
        ITS_MAX_ASSET_SIZE;
        ITS_NUM_ASSETS;
        ITS_BUF_SIZE;
        ITS_STACK_SIZE
    "
    )

############################ Secure API ########################################

target_sources(tfm_sprt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_its_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_its_secure_api.c
    PROPERTIES
        COMPILE_FLAGS
            $<$<C_COMPILER_ID:ARMClang>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:GNU>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:IAR>:>
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_its
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
)
