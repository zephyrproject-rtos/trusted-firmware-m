#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(${CC312_LIB_TARGET}
    PUBLIC
        include
        include/internal
        .
)

target_sources(${CC312_LIB_TARGET}
    PRIVATE
        src/cc3xx_psa_hash.c
        src/cc3xx_psa_cipher.c
        src/cc3xx_psa_mac.c
        src/cc3xx_psa_entropy.c
        src/cc3xx_psa_key_agreement.c

        src/cc3xx_internal_asn1_util.c
        src/cc3xx_internal_ecc_util.c
        src/cc3xx_internal_rsa_util.c
        src/cc3xx_psa_asymmetric_signature.c
        src/cc3xx_psa_key_generation.c
        src/cc3xx_internal_drbg_util.c

        src/cc3xx_internal_aes.c
        src/cc3xx_internal_chacha20.c
        src/cc3xx_internal_ecdh.c

        src/aead/cc3xx_psa_aead.c
        src/aead/cc3xx_psa_aead_ccm.c
        src/aead/cc3xx_psa_aead_gcm.c
        src/aead/cc3xx_psa_aead_chacha20_poly1305.c
)

target_compile_definitions(${CC312_LIB_TARGET}
    PUBLIC
        MBEDTLS_PSA_CRYPTO_DRIVERS
        PSA_CRYPTO_DRIVER_CC3XX
)
