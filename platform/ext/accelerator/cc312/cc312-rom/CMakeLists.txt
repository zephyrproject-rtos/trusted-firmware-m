#-------------------------------------------------------------------------------
# Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CC3XX_TARGET_NAME ${CC3XX_TARGET_PREFIX}cc312_rom)

add_library(${CC3XX_TARGET_NAME} STATIC)

if (NOT DEFINED CC3XX_PLATFORM_INTERFACE)
    message(FATAL "When building cc312-rom, the platform dependency must be specified")
endif()

target_sources(${CC3XX_TARGET_NAME}
    PRIVATE
        ./cc3xx_lcs.c
        ./cc3xx_otp.c
        ./cc3xx_rng.c
        ./cc3xx_hash.c
        ./cc3xx_aes.c
        ./cc3xx_dma.c
        ./cc3xx_init.c
        ./cc3xx_engine_state.c
        ./cc3xx_stdlib.c
        ./cc3xx_chacha.c
        ./cc3xx_kdf.c
        ./cc3xx_pka.c
        ./cc3xx_poly1305.c
        ./cc3xx_hmac.c
        ./cc3xx_drbg_ctr.c
        ./cc3xx_drbg_hash.c
        ./cc3xx_drbg_hmac.c
        ./cc3xx_drbg.c
        ./cc3xx_ecdsa.c
        ./cc3xx_ec.c
        ./cc3xx_ec_curve_data.c
        ./cc3xx_ec_weierstrass.c
        ./cc3xx_ec_projective_point.c
        ./cc3xx_ecdh.c
)

# This file is the performance-limit for ECDSA, so should be compiled with the
# highest possible optimizations for speed, even when we are optimizing for
# code size.
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if (${BUILD_TYPE_LOWER} STREQUAL "release" OR ${BUILD_TYPE_LOWER} STREQUAL "minsizerel")
    set_source_files_properties(./cc3xx_pka.c
        PROPERTIES COMPILE_FLAGS -Ofast
    )
endif()

target_include_directories(${CC3XX_TARGET_NAME}
    PUBLIC
        .
)

target_link_libraries(${CC3XX_TARGET_NAME}
    PRIVATE
        ${CC3XX_PLATFORM_INTERFACE}
)

add_subdirectory(psa_driver_api)
