#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(PLATFORM_DUMMY_ATTEST_HAL           FALSE       CACHE BOOL      "Use dummy boot hal implementation. Should not be used in production.")
set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")

if(CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "ENABLED")
    set(PLATFORM_DUMMY_CRYPTO_KEYS      FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production.")
endif()

set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/musca_b1/generated_file_list.yaml  CACHE PATH "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)
