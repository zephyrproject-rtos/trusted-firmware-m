#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if(BL2)
    add_library(crypto_hw_bl2 STATIC)
endif()
add_library(crypto_hw_crypto_service STATIC)

add_subdirectory(${CRYPTO_HW_ACCELERATOR_TYPE})

################################ BL2 ###########################################

if(BL2)
    target_include_directories(crypto_hw_bl2
        PUBLIC
            interface
    )

    target_compile_definitions(crypto_hw_bl2
        PUBLIC
            CRYPTO_HW_ACCELERATOR
    )

    target_link_libraries(platform_bl2
        PRIVATE
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:crypto_hw_bl2>
    )
endif()

############################ Crypto Service ####################################

target_include_directories(crypto_hw_crypto_service
    PUBLIC
        interface
)

target_compile_definitions(crypto_hw_crypto_service
    PUBLIC
        CRYPTO_HW_ACCELERATOR
)

target_link_libraries(platform_s
    PRIVATE
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:crypto_hw_crypto_service>
)
