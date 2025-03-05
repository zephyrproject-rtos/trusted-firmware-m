#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CC3XX_TEST_TARGET_NAME ${CC3XX_TARGET_NAME}_tests)

add_library(${CC3XX_TEST_TARGET_NAME} INTERFACE)

target_sources(${CC3XX_TEST_TARGET_NAME}
    INTERFACE
        ./src/cc3xx_test_main.c
        ./src/cc3xx_test_hash.c
        ./src/low_level_driver/test_lowlevel_hash.c
        ./src/cc3xx_test_aes.c
        ./src/low_level_driver/test_lowlevel_aes.c
        ./src/cc3xx_test_chacha.c
        ./src/low_level_driver/test_lowlevel_chacha.c
        ./src/cc3xx_test_pka.c
        ./src/cc3xx_test_ecc.c
        ./src/cc3xx_test_ecdsa.c
        ./src/cc3xx_test_drbg.c
        ./src/cc3xx_test_utils.c
)

target_include_directories(${CC3XX_TEST_TARGET_NAME}
    INTERFACE
        ./include
        ./src
        ./src/low_level_driver
)

target_compile_definitions(${CC3XX_TEST_TARGET_NAME}
    INTERFACE
        $<$<BOOL:${TEST_CC3XX}>:TEST_CC3XX>
)

target_link_libraries(${CC3XX_TEST_TARGET_NAME}
    INTERFACE
        ${CC3XX_TARGET_NAME}
        ${CC3XX_PLATFORM_INTERFACE}
)
