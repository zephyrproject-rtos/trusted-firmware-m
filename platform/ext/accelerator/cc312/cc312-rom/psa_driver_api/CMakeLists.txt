#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CC3XX_PSA_TARGET_NAME ${CC3XX_TARGET_NAME}_psa_driver_api)

add_library(${CC3XX_PSA_TARGET_NAME} STATIC EXCLUDE_FROM_ALL)

target_sources(${CC3XX_PSA_TARGET_NAME}
    PRIVATE
        ./src/cc3xx_psa_entropy.c
        ./src/cc3xx_psa_hash.c
        ./src/cc3xx_psa_init.c
        ./src/cc3xx_psa_mac.c
        ./src/cc3xx_psa_random.c
        ./src/cc3xx_psa_cipher.c
        ./src/cc3xx_psa_aead.c
        ./src/cc3xx_psa_asymmetric_signature.c
        ./src/cc3xx_psa_asymmetric_encryption.c
        ./src/cc3xx_psa_key_generation.c
        ./src/cc3xx_psa_key_agreement.c
        ./src/cc3xx_internal_cipher.c
        ./src/cc3xx_misc.c
)

target_include_directories(${CC3XX_PSA_TARGET_NAME}
    PUBLIC
        .
        include
)

target_link_libraries(${CC3XX_PSA_TARGET_NAME}
    PUBLIC
        ${CC3XX_TARGET_NAME}
        ${CC3XX_PLATFORM_INTERFACE}
)

# FixMe: Until the PSA Crypto core of mbed TLS is upgraded to integrate
#        cc3xx_random entry points into the driver wrapper, we must use
#        the existing mechanism based on the following definition and
#        associated mbedtls_psa_external_get_random() function
#
target_compile_definitions(${CC3XX_PSA_TARGET_NAME}
    PUBLIC
        MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
)
