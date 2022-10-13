#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_PROFILE                         profile_small CACHE STRING  "Profile to use")
set(TFM_ISOLATION_LEVEL                 1           CACHE STRING    "Isolation level")

########################## BL2 #################################################

set(MCUBOOT_IMAGE_NUMBER                1           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately")

############################ Partitions ########################################

set(TFM_PARTITION_PROTECTED_STORAGE     OFF         CACHE BOOL      "Enable Protected Storage partition")

set(ITS_BUF_SIZE                        32          CACHE STRING    "Size of the ITS internal data transfer buffer (defaults to ITS_MAX_ASSET_SIZE if not set)")

set(CRYPTO_CONC_OPER_NUM                4           CACHE STRING    "The max number of concurrent operations that can be active (allocated) at any time in Crypto")
# Profile Small assigns a much smller heap size for backend crypto library as
# asymmetric cryptography is not enabled and multi-part operations are enabled
# only.
# Assign 0x100 bytes for each operation and totally 0x800 byets for max 4
# concurrent operation as set in CRYPTO_CONC_OPER_NUM above
set(CRYPTO_ENGINE_BUF_SIZE              0x400       CACHE STRING    "Heap size for the crypto backend")
set(CRYPTO_ASYM_SIGN_MODULE_DISABLED    ON          CACHE BOOL      "Disable PSA Crypto asymmetric key signature module")
set(CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED ON          CACHE BOOL      "Disable PSA Crypto asymmetric key encryption module")
set(CRYPTO_SINGLE_PART_FUNCS_DISABLED   ON          CACHE BOOL      "Only enable multi-part operations in Hash, MAC, AEAD and symmetric ciphers, to optimize memory footprint in resource-constrained devices")

set(SYMMETRIC_INITIAL_ATTESTATION       ON          CACHE BOOL      "Use symmetric crypto for inital attestation")

set(TFM_PARTITION_PLATFORM              OFF         CACHE BOOL      "Enable Platform partition")

set(TFM_PARTITION_FIRMWARE_UPDATE       OFF         CACHE BOOL      "Enable firmware update partition")

################################## Dependencies ################################

set(TFM_MBEDCRYPTO_CONFIG_PATH              "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/tfm_mbedcrypto_config_profile_small.h" CACHE PATH "Config to use for Mbed Crypto")
set(TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH   "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/crypto_config_profile_small.h" CACHE PATH "Config to use psa crypto setting for Mbed Crypto.")

# Enable SFN model in Profile Small by default.
set(CONFIG_TFM_SPM_BACKEND          "SFN"       CACHE STRING    "The SPM backend [IPC, SFN]")
set(PSA_FRAMEWORK_HAS_MM_IOVEC      ON          CACHE BOOL      "Enable MM-IOVEC")
set(CONFIG_TFM_CONN_HANDLE_MAX_NUM  3           CACHE STRING    "The maximal number of secure services that are connected or requested at the same time")
