#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


set(PLATFORM_DUMMY_ATTEST_HAL           FALSE       CACHE BOOL      "Use dummy boot hal implementation. Should not be used in production.")
set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")

if (NOT FORWARD_PROT_MSG)
    set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
    if(CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "ENABLED")
        set(PLATFORM_DUMMY_CRYPTO_KEYS      FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production.")
        # Musca-B1 with OTP enabled is provisioned with a random Initial
        # Attestation key generated in runtime.
        # The public key data is not pre-defined and therefore it can only be
        # retrieved in runtime for test.
        set(ATTEST_TEST_GET_PUBLIC_KEY      ON          CACHE BOOL      "Require to retrieve Initial Attestation public in runtime for test purpose")
    endif()
    set(MCUBOOT_DATA_SHARING                ON         CACHE BOOL      "Add sharing of application specific data using the same shared data area as for the measured boot")
    set(TFM_PARTITION_FIRMWARE_UPDATE       ON         CACHE BOOL      "Enable firmware update partition")
    set(BL0 OFF)
else()
    set(MCUBOOT_IMAGE_NUMBER                1           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately")
    set(MCUBOOT_MEASURED_BOOT               OFF         CACHE BOOL      "Add boot measurement values to boot status. Used for initial attestation token")
    set(TFM_PSA_API                         ON          CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode")
    set(TFM_PARTITION_PROTECTED_STORAGE     OFF         CACHE BOOL      "Enable Protected Storage partition")
    set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE OFF      CACHE BOOL      "Enable Internal Trusted Storage partition")
    set(TFM_PARTITION_CRYPTO                OFF         CACHE BOOL      "Enable Crypto partition")
    set(TFM_PARTITION_INITIAL_ATTESTATION   OFF         CACHE BOOL      "Enable Initial Attestation partition")
    set(TFM_PARTITION_PLATFORM              OFF         CACHE BOOL      "Enable Platform partition")
    set(TFM_PARTITION_PSA_PROXY             ON          CACHE BOOL      "Enable PSA Proxy partition")
    # In case of forwarding, there is no CRYPTO partition compiled,
    # thus no need for crypto hw accelerator.
    set(CRYPTO_HW_ACCELERATOR               OFF         CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
    set(PS_TEST_NV_COUNTERS                 OFF         CACHE BOOL      "Use the test NV counters to test Protected Storage rollback scenarios")
    set(BL0 ON)
endif()

set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/arm/musca_b1/generated_file_list.yaml  CACHE PATH "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)

set(PSA_API_TEST_TARGET                 "musca_b1"   CACHE STRING    "Target to use when building the PSA API tests")
