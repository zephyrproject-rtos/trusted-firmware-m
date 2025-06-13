#-------------------------------------------------------------------------------
# Copyright (C) 2025 Analog Devices, Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(${PLATFORM_DIR}/ext/target/${TFM_PLATFORM}/accelerator/CMakeLists.txt)
if(BL2)
    target_link_libraries(bl2_mbedcrypto
        PUBLIC
            bl2_crypto_hw
    )

    target_compile_definitions(bl2_mbedcrypto
        PUBLIC
            CRYPTO_HW_ACCELERATOR
    )

    target_compile_options(bl2_mbedcrypto
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
    )
endif()

if (TFM_PARTITION_CRYPTO)
    target_link_libraries(crypto_service_mbedcrypto
        PUBLIC
            crypto_service_crypto_hw
    )

    target_compile_definitions(crypto_service_mbedcrypto
        PUBLIC
            CRYPTO_HW_ACCELERATOR
    )

    target_compile_options(crypto_service_mbedcrypto
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
    )
endif()
