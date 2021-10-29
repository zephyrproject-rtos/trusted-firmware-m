#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(${CC312_LIB_TARGET}
    PUBLIC
        .
        include
)

target_sources(${CC312_LIB_TARGET}
    PRIVATE
        src/cc3xx_hash.c
        src/cc3xx_cipher.c
        src/cc3xx_mac.c
        src/cc3xx_entropy.c
        src/cc3xx_key_agreement.c
        src/cc3xx_asymmetric_signature.c
        src/cc3xx_asymmetric_encryption.c
        src/cc3xx_key_generation.c
        src/cc3xx_aead.c
        # Internal sources
        src/cc3xx_internal_asn1_util.c
        src/cc3xx_internal_ecc_util.c
        src/cc3xx_internal_rsa_util.c
        src/cc3xx_internal_drbg_util.c
        src/cc3xx_internal_hash_util.c
        src/cc3xx_internal_aes.c
        src/cc3xx_internal_chacha20.c
        src/cc3xx_internal_ecdh.c
        src/cc3xx_internal_ccm.c
        src/cc3xx_internal_gcm.c
        src/cc3xx_internal_chacha20_poly1305.c
)

target_compile_definitions(${CC312_LIB_TARGET}
    PUBLIC
        MBEDTLS_PSA_CRYPTO_DRIVERS
        PSA_CRYPTO_DRIVER_CC3XX
)
