#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")

if(CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "ENABLED")
    set(PLATFORM_DUMMY_CRYPTO_KEYS      FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production.")
    # Musca-S1 with OTP enabled is provisioned with a random Initial Attestation
    # key generated in runtime.
    # The public key data is not pre-defined and therefore it can only be
    # retrieved in runtime for test.
    set(ATTEST_TEST_GET_PUBLIC_KEY      ON          CACHE BOOL      "Require to retrieve Initial Attestation public in runtime for test purpose")
endif()
