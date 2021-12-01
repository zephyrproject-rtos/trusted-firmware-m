#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
# Allow linking to things 'upwards' in the directory tree (in this case bl2 / tfm_psa_rot_partition_crypto)
cmake_policy(SET CMP0079 NEW)
# Allow relative paths
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED CC312_PATH)
    set(CC312_PATH ../../../../lib/ext/cryptocell-312-runtime CACHE PATH "Path to CC312 lib")
endif()

if(BL2)
    target_compile_definitions(platform_bl2
        PRIVATE
            CRYPTO_HW_ACCELERATOR
    )
endif()

################################ OTP ###########################################

target_sources(platform_s
    PRIVATE
        $<$<NOT:$<BOOL:${PLATFORM_DEFAULT_OTP}>>:${CMAKE_CURRENT_SOURCE_DIR}/otp_cc312.c>
)

if(BL2)
    target_sources(platform_bl2
        PRIVATE
        $<$<NOT:$<BOOL:${PLATFORM_DEFAULT_OTP}>>:${CMAKE_CURRENT_SOURCE_DIR}/otp_cc312.c>
    )
endif()

################################ BL2 ###########################################

if(BL2)
    target_sources(bl2_crypto_hw
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/cc312.c
    )

    target_include_directories(bl2_crypto_hw
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(bl2_crypto_hw
        PUBLIC
            bl2_cc312
        PRIVATE
            bl2_mbedcrypto
            platform_bl2
    )

    set(CC312_LIB_TARGET bl2_cc312)
    set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
    add_subdirectory(${CC312_PATH} ${CMAKE_CURRENT_BINARY_DIR}/bl2_cc312)
    set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

    target_sources(bl2_cc312
        PUBLIC
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_log.c>
    )

    # Adding two targets as link-time dependencies of each other seems bad, but
    # in reality it just means that they'll share headers and compile defs.
    target_link_libraries(bl2_cc312
        PRIVATE
            bl2_mbedcrypto
            platform_bl2
        PUBLIC
            platform_common_interface
    )
    target_link_libraries(bl2_mbedcrypto
        PRIVATE
            bl2_cc312
        PUBLIC
            bl2_crypto_hw
    )

    # boot_hal depends on crypto_hw_accelerator abstractions
    target_link_libraries(platform_bl2
        PRIVATE
            bl2_crypto_hw
    )

    target_link_libraries(cc312_cdmpu
        INTERFACE
            bl2_mbedcrypto
    )

    target_compile_definitions(bl2_mbedcrypto
        PUBLIC
            CRYPTO_HW_ACCELERATOR
            MBEDTLS_ECDH_LEGACY_CONTEXT
    )

    target_compile_options(bl2_mbedcrypto
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
            $<$<C_COMPILER_ID:ARMClang>:-Wno-unused-parameter>
    )

    target_compile_options(bl2_cc312
        PRIVATE
            ${BL2_COMPILER_CP_FLAG}
    )

    target_compile_options(bl2_crypto_hw
        PRIVATE
            ${BL2_COMPILER_CP_FLAG}
    )

    target_compile_options(cc312_cdmpu
        INTERFACE
            ${BL2_COMPILER_CP_FLAG}
    )
endif()

############################ Crypto Service ####################################

target_sources(crypto_service_crypto_hw
    PRIVATE
        cc312.c
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_log.c>
)

target_include_directories(crypto_service_crypto_hw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(crypto_service_crypto_hw
    PUBLIC
        crypto_service_cc312
    PRIVATE
        crypto_service_mbedcrypto
        platform_s
)

set(CC312_LIB_TARGET crypto_service_cc312)
set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
add_subdirectory(${CC312_PATH} ${CMAKE_CURRENT_BINARY_DIR}/crypto_service_cc312)
set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

target_sources(crypto_service_cc312
    PUBLIC
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:${CMAKE_CURRENT_SOURCE_DIR}/cc312_log.c>
)

# Adding two targets as link-time dependencies of each other seems bad, but
# in reality it just means that they'll share headers and compile defs.
target_link_libraries(crypto_service_cc312
    PRIVATE
        crypto_service_mbedcrypto
    PUBLIC
        platform_s
)
target_link_libraries(crypto_service_mbedcrypto
    PRIVATE
        crypto_service_cc312
    PUBLIC
        crypto_service_crypto_hw
)

target_compile_definitions(crypto_service_mbedcrypto
    PUBLIC
        CRYPTO_HW_ACCELERATOR
        MBEDTLS_ECDH_LEGACY_CONTEXT
)

target_compile_options(crypto_service_mbedcrypto
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
        $<$<C_COMPILER_ID:ARMClang>:-Wno-unused-parameter>
)
