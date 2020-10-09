#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Bootloader" VERSION 0.1.0 LANGUAGES C ASM)

add_executable(bl2
    src/security_cnt.c
    src/flash_map.c
)

add_subdirectory(ext/mcuboot)

set_target_properties(bl2
    PROPERTIES
        SUFFIX ".axf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(bl2
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(bl2
    PRIVATE
        platform_bl2
        tfm_boot_status
        mbedcrypto_bl2
)

target_link_options(bl2
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-Map=${CMAKE_BINARY_DIR}/bin/bl2.map>
        $<$<C_COMPILER_ID:ARMClang>:--map>
)

add_convert_to_bin_target(bl2)

############################### BOOT HAL # #####################################

add_library(bl2_hal INTERFACE)

target_include_directories(bl2_hal
    INTERFACE
        include
)

############################### MBEDCRYPTO #####################################

add_library(mbedcrypto_bl2_config INTERFACE)

target_compile_definitions(mbedcrypto_bl2_config
    INTERFACE
        $<$<STREQUAL:${MCUBOOT_SIGNATURE_TYPE},RSA>:MCUBOOT_SIGN_RSA>
        $<$<STREQUAL:${MCUBOOT_SIGNATURE_TYPE},RSA>:MCUBOOT_SIGN_RSA_LEN=${MCUBOOT_SIGNATURE_KEY_LEN}>
        MBEDTLS_CONFIG_FILE="$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/mcuboot/include/mcuboot-mbedtls-cfg.h>"
        # Workaround for https://github.com/ARMmbed/mbedtls/issues/1077
        $<$<OR:$<STREQUAL:${CMAKE_SYSTEM_ARCHITECTURE},armv8-m.base>,$<STREQUAL:${CMAKE_SYSTEM_ARCHITECTURE},armv6-m>>:MULADDC_CANNOT_USE_R7>
)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(ENABLE_TESTING OFF)
set(ENABLE_PROGRAMS OFF)
set(MBEDTLS_FATAL_WARNINGS OFF)
set(ENABLE_DOCS OFF)
set(INSTALL_MBEDTLS OFF)
set(INSTALL_MBEDTLS_HEADERS OFF)

# Current variables
set(mbedcrypto_static_target mbedcrypto_bl2)
set(mbedcrypto_lib_target mbedcrypto_lib_bl2)
# Future variables
set(lib_target lib_bl2)
set(mbedcrypto_target mbedcrypto_bl2)
set(mbedtls_target mbedtls_bl2)
set(mbedx509_target mbedx509_bl2)

# Mbedcrypto is quite a large lib, and it uses too much memory for it to be
# reasonable to build it in debug info. As a compromise, if `debug` build type
# is selected mbedcrypto will build under `relwithdebinfo` which preserved debug
# symbols whild optimizing space.
set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
add_subdirectory(${MBEDCRYPTO_PATH} ${CMAKE_CURRENT_BINARY_DIR}/mbedcrypto)
set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

if(NOT TARGET mbedcrypto_bl2)
    message(FATAL_ERROR "Target mbedcrypto_bl2 does not exist. Have the patches in ${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto been applied to the mbedcrypto repo at ${MBEDCRYPTO_PATH} ?
    Hint: The command might be `cd ${MBEDCRYPTO_PATH} && git apply ${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/*.patch`")
endif()

set_target_properties(${mbedtls_target} ${mbedx509_target}
    PROPERTIES
        EXCLUDE_FROM_ALL TRUE
)

target_link_libraries(mbedcrypto_bl2
    PUBLIC
        mbedcrypto_bl2_config
)
