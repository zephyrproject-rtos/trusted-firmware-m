#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        fip_parser.c
        host_flash_atu.c
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/gpt.c>
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/fwu_metadata.c>
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/../soft_crc/soft_crc.c>
)

target_include_directories(platform_bl2
    PUBLIC
        .
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/../soft_crc>
)

#========================= Platform BL1 =======================================#

target_sources(platform_bl1_1
    PRIVATE
        fip_parser.c
        host_flash_atu.c
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/gpt.c>
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/fwu_metadata.c>
        $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/../soft_crc/soft_crc.c>
)

target_include_directories(platform_bl1_1_interface
    INTERFACE
        .
)

target_include_directories(platform_bl1_1
   PRIVATE
       $<$<BOOL:${RSE_GPT_SUPPORT}>:${CMAKE_CURRENT_SOURCE_DIR}/../soft_crc>
)
