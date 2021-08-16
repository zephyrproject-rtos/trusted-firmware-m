#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_PROFILE                         profile_small CACHE STRING  "Profile to use")
set(TFM_PSA_API                         OFF         CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode")
set(TFM_ISOLATION_LEVEL                 1           CACHE STRING    "Isolation level")

########################## BL2 #################################################

set(MCUBOOT_IMAGE_NUMBER                1           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately")

############################ Partitions ########################################

set(TFM_PARTITION_PROTECTED_STORAGE     OFF         CACHE BOOL      "Enable Protected Storage partition")

set(ITS_BUF_SIZE                        32          CACHE STRING    "Size of the ITS internal data transfer buffer (defaults to ITS_MAX_ASSET_SIZE if not set)")

set(CRYPTO_CONC_OPER_NUM                4           CACHE STRING    "The max number of concurrent operations that can be active (allocated) at any time in Crypto")
# Profile Small assigns a much smller heap size for backend crypto library as
# asymmetric cryptography is not enabled.
# Assign 0x200 bytes for each operation and totally 0x800 byets for max 4
# concurrent operation as set in CRYPTO_CONC_OPER_NUM above
set(CRYPTO_ENGINE_BUF_SIZE              0x800       CACHE STRING    "Heap size for the crypto backend")
set(CRYPTO_ASYM_SIGN_MODULE_DISABLED    ON          CACHE BOOL      "Disable PSA Crypto asymmetric key signature module")
set(CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED ON          CACHE BOOL      "Disable PSA Crypto asymmetric key encryption module")
# TODO CRYPTO_KEY_ID_ENCODES_OWNER shall be aligned with underlying crypto
# library key ID encoding configuration
set(CRYPTO_KEY_ID_ENCODES_OWNER         OFF         CACHE BOOL      "Encode client ID into Crypto PSA key ID")

set(SYMMETRIC_INITIAL_ATTESTATION       ON          CACHE BOOL      "Use symmetric crypto for inital attestation")

set(TFM_PARTITION_PLATFORM              OFF         CACHE BOOL      "Enable Platform partition")

set(TFM_PARTITION_FIRMWARE_UPDATE       OFF         CACHE BOOL      "Enable firmware update partition")

set(TFM_PARTITION_AUDIT_LOG             OFF         CACHE BOOL      "Enable Audit Log partition")

################################## Tests #######################################

# TFM_CRYPTO_TEST_ALG_CCM is the only test mode left enabled, to go with the
# reduced crypto config
set(TFM_CRYPTO_TEST_ALG_CBC             OFF         CACHE BOOL      "Test CBC cryptography mode")
set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")
set(TFM_CRYPTO_TEST_ALG_CTR             OFF         CACHE BOOL      "Test CTR cryptography mode")
set(TFM_CRYPTO_TEST_ALG_GCM             OFF         CACHE BOOL      "Test GCM cryptography mode")
set(TFM_CRYPTO_TEST_ALG_SHA_512         OFF         CACHE BOOL      "Test SHA-512 cryptography algorithm")
set(TFM_CRYPTO_TEST_HKDF                OFF         CACHE BOOL      "Test the HKDF key derivation algorithm")
set(TFM_CRYPTO_TEST_ECDH                OFF         CACHE BOOL      "Test the ECDH key agreement algorithm")

################################## Dependencies ################################

set(TFM_MBEDCRYPTO_CONFIG_PATH          "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/tfm_mbedcrypto_config_profile_small.h" CACHE PATH "Config to use for Mbed Crypto")

