#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_PROFILE                         profile_medium CACHE STRING    "Profile to use")
set(TFM_ISOLATION_LEVEL                 2           CACHE STRING    "Isolation level")

############################ Partitions ########################################

set(ITS_BUF_SIZE                        32          CACHE STRING    "Size of the ITS internal data transfer buffer (defaults to ITS_MAX_ASSET_SIZE if not set)")

set(PS_CRYPTO_AEAD_ALG                  PSA_ALG_CCM CACHE STRING    "The AEAD algorithm to use for authenticated encryption in protected storage")

set(CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED ON          CACHE BOOL      "Disable PSA Crypto asymmetric key encryption module")

################################## Dependencies ################################

set(TFM_MBEDCRYPTO_CONFIG_PATH              "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/tfm_mbedcrypto_config_profile_medium.h" CACHE PATH "Config to use for Mbed Crypto")
set(TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH   "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/crypto_config_profile_medium.h" CACHE PATH "Config to use psa crypto setting for Mbed Crypto.")

set(CONFIG_TFM_SPM_BACKEND          "IPC"       CACHE STRING    "The SPM backend [IPC, SFN]")
