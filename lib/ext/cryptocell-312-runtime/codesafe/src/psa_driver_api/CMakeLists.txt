#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(${CC312_LIB_TARGET}
    PUBLIC
        include
        .
)

target_sources(${CC312_LIB_TARGET}
    PRIVATE
        src/cc3xx_psa_hash.c
)

target_compile_definitions(${CC312_LIB_TARGET}
    PUBLIC
    MBEDTLS_PSA_CRYPTO_DRIVERS
    PSA_CRYPTO_DRIVER_CC3XX
)
