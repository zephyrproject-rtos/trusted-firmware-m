#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/arm/mps2/an521/generated_file_list.yaml  CACHE PATH "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)

if(TFM_PSA_API)
    set(TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/mbedtls_an521_conf.h CACHE FILEPATH "Config to append to standard Mbed Crypto config, used by platforms to configure feature support")
    set(PLATFORM_DUMMY_NV_SEED        TRUE         CACHE BOOL      "Use dummy NV seed implementation. Should not be used in production.")
endif()
