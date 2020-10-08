#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
# Allow linking to things 'upwards' in the directory tree (in this case bl2 / tfm_partition_crypto)
cmake_policy(SET CMP0079 NEW)
# Allow relative paths
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED CC312_PATH)
    set(CC312_PATH ../../../../lib/ext/cryptocell-312-runtime CACHE PATH "Path to CC312 lib")
endif()

target_compile_definitions(platform_bl2
    PRIVATE
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:CRYPTO_HW_ACCELERATOR_OTP_ENABLED>
        CRYPTO_HW_ACCELERATOR
)

################################ BL2 ###########################################
if(BL2)
    target_sources(crypto_hw_bl2
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/cc312.c
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_log.c>
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},PROVISIONING>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_provisioning.c>
    )

    target_include_directories(crypto_hw_bl2
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(crypto_hw_bl2
        PUBLIC
            cc312_bl2
        PRIVATE
            mbedcrypto_bl2
            platform_bl2
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},PROVISIONING>:cc312_cdmpu>
    )

    set(CC312_LIB_TARGET cc312_bl2)
    set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
    add_subdirectory(${CC312_PATH} ${CMAKE_CURRENT_BINARY_DIR}/cc312_bl2)
    set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

    # Adding two targets as link-time dependencies of each other seems bad, but
    # in reality it just means that they'll share headers and compile defs.
    target_link_libraries(cc312_bl2
        PRIVATE
            mbedcrypto_bl2
    )
    target_link_libraries(mbedcrypto_bl2
        PRIVATE
            cc312_bl2
        PUBLIC
            crypto_hw_bl2
    )

    #Link utils to PAL for logging
    target_link_libraries(cc312_pal
        PRIVATE
            crypto_hw_bl2
    )

    target_link_libraries(cc312_cdmpu
        PRIVATE
            mbedcrypto_bl2
    )

    target_compile_definitions(mbedcrypto_bl2
        PUBLIC
            CRYPTO_HW_ACCELERATOR
            MBEDTLS_ECDH_LEGACY_CONTEXT
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},PROVISIONING>:CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING>
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:CRYPTO_HW_ACCELERATOR_OTP_ENABLED>
    )

    target_compile_options(mbedcrypto_bl2
        PRIVATE
            -Wno-unused-parameter
    )

    #Include platform specific defs (registers etc)
    target_include_directories(cc312_bl2
        BEFORE
        PUBLIC
            ${CC312_PATH}/shared/hw/include/${TFM_PLATFORM}
    )

    target_include_directories(cc312_cdmpu
        BEFORE
        PUBLIC
        ${CC312_PATH}/shared/hw/include/${TFM_PLATFORM}
    )
endif()

############################ Crypto Service ####################################

target_sources(crypto_hw_crypto_service
    PRIVATE
        cc312.c
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_log.c>
)

target_include_directories(crypto_hw_crypto_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(crypto_hw_crypto_service
    PUBLIC
        cc312_crypto_service
    PRIVATE
        mbedcrypto_crypto_service
        platform_s
)

target_link_libraries(platform_s
    PRIVATE
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:crypto_hw_crypto_service>
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:cc312_crypto_service>
)

set(CC312_LIB_TARGET cc312_crypto_service)
set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
add_subdirectory(${CC312_PATH} ${CMAKE_CURRENT_BINARY_DIR}/cc312_crypto_service)
set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

# Adding two targets as link-time dependencies of each other seems bad, but
# in reality it just means that they'll share headers and compile defs.
target_link_libraries(cc312_crypto_service
    PRIVATE
        mbedcrypto_crypto_service
)
target_link_libraries(mbedcrypto_crypto_service
    PRIVATE
        cc312_crypto_service
    PUBLIC
        crypto_hw_crypto_service
)

target_compile_definitions(mbedcrypto_crypto_service
    PUBLIC
        CRYPTO_HW_ACCELERATOR
        MBEDTLS_ECDH_LEGACY_CONTEXT
        $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:CRYPTO_HW_ACCELERATOR_OTP_ENABLED>
)

target_compile_options(mbedcrypto_crypto_service
    PRIVATE
        -Wno-unused-parameter
)

#Include platform specific defs (registers etc)
target_include_directories(cc312_crypto_service
    BEFORE
    PUBLIC
    ${CC312_PATH}/shared/hw/include/${TFM_PLATFORM}
)
