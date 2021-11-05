#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2021 STMicroelectronics. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## BL2 #################################################

set(MCUBOOT_IMAGE_NUMBER                2           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately")
set(BL2_TRAILER_SIZE                    0x2000      CACHE STRING    "Trailer size")

################################## Dependencies ################################
set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
set(CRYPTO_NV_SEED                      OFF         CACHE BOOL      "Use stored NV seed to provide entropy")
set(MBEDCRYPTO_BUILD_TYPE               minsizerel  CACHE STRING    "Build type of Mbed Crypto library")
set(TFM_DUMMY_PROVISIONING              OFF         CACHE BOOL      "Provision with dummy values. NOT to be used in production")              
set(PLATFORM_DEFAULT_OTP_WRITEABLE      OFF         CACHE BOOL      "Use on chip flash with write support")
set(PLATFORM_DEFAULT_NV_COUNTERS        OFF         CACHE BOOL      "Use default nv counter implementation.")
set(PS_CRYPTO_AEAD_ALG                  PSA_ALG_GCM CACHE STRING    "The AEAD algorithm to use for authenticated encryption in Protected Storage")