#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_CRYPTO)
    return()
endif()

cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_partition_crypto STATIC)

target_sources(tfm_partition_crypto
    PRIVATE
        crypto_init.c
        crypto_alloc.c
        crypto_cipher.c
        crypto_hash.c
        crypto_mac.c
        crypto_key.c
        crypto_aead.c
        crypto_asymmetric.c
        crypto_key_derivation.c
)

target_include_directories(tfm_partition_crypto
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Linking to external interfaces
target_link_libraries(tfm_partition_crypto
    PRIVATE
        tfm_secure_api
        platform_s
        mbedcrypto_crypto_service
        psa_interface
)
target_compile_definitions(tfm_partition_crypto
    PRIVATE
    $<$<BOOL:${CRYPTO_KEY_MODULE_DISABLED}>:CRYPTO_KEY_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_AEAD_MODULE_DISABLED}>:CRYPTO_AEAD_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_MAC_MODULE_DISABLED}>:CRYPTO_MAC_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_CIPHER_MODULE_DISABLED}>:CRYPTO_CIPHER_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_HASH_MODULE_DISABLED}>:CRYPTO_HASH_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_GENERATOR_MODULE_DISABLED}>:CRYPTO_GENERATOR_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_ASYMMETRIC_MODULE_DISABLED}>:CRYPTO_ASYMMETRIC_MODULE_DISABLED>
    $<$<BOOL:${CRYPTO_ENGINE_BUF_SIZE}>:CRYPTO_ENGINE_BUF_SIZE=${CRYPTO_ENGINE_BUF_SIZE}>
    $<$<BOOL:${CRYPTO_CONC_OPER_NUM}>:CRYPTO_CONC_OPER_NUM=${CRYPTO_CONC_OPER_NUM}>
    $<$<AND:$<NOT:$<BOOL:${TFM_PSA_API}>>,$<BOOL:${CRYPTO_IOVEC_BUFFER_SIZE}>>:CRYPTO_IOVEC_BUFFER_SIZE=${CRYPTO_IOVEC_BUFFER_SIZE}>
)

############################ Secure API ########################################

target_sources(tfm_secure_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tfm_crypto_secure_api.c
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_partition_crypto_secure_api.c
    PROPERTIES
        COMPILE_FLAGS -Wno-implicit-function-declaration
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_partition_crypto
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_CRYPTO
)

############################### MBEDCRYPTO #####################################

add_library(mbedcrypto_crypto_service_config INTERFACE)

target_compile_definitions(mbedcrypto_crypto_service_config
    INTERFACE
        MBEDTLS_CONFIG_FILE="${TFM_MBEDCRYPTO_CONFIG_PATH}"
        $<$<BOOL:${TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH}>:MBEDTLS_USER_CONFIG_FILE="${TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH}">
        PSA_CRYPTO_SECURE
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
set(mbedcrypto_static_target mbedcrypto_crypto_service)
set(mbedcrypto_lib_target mbedcrypto_lib_crypto_service)
# Future variables
set(lib_target lib_crypto_service)
set(mbedcrypto_target mbedcrypto_crypto_service)
set(mbedtls_target mbedtls_crypto_service)
set(mbedx509_target mbedx509_crypto_service)

# Mbedcrypto is quite a large lib, and it uses too much memory for it to be
# reasonable to build it in debug info. As a compromise, if `debug` build type
# is selected mbedcrypto will build under `relwithdebinfo` which preserved debug
# symbols whild optimizing space.
set(SAVED_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${MBEDCRYPTO_BUILD_TYPE})
add_subdirectory(${MBEDCRYPTO_PATH} ${CMAKE_CURRENT_BINARY_DIR}/mbedcrypto)
set(CMAKE_BUILD_TYPE ${SAVED_BUILD_TYPE} CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)

if(NOT TARGET mbedcrypto_crypto_service)
    message(FATAL_ERROR "Target mbedcrypto_crypto_service does not exist. Have the patches in ${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto been applied to the mbedcrypto repo at ${MBEDCRYPTO_PATH} ?
    Hint: The command might be `cd ${MBEDCRYPTO_PATH} && git apply ${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/*.patch`")
endif()

set_target_properties(${mbedtls_target} ${mbedx509_target}
    PROPERTIES
        EXCLUDE_FROM_ALL TRUE
)

target_include_directories(mbedcrypto_crypto_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(mbedcrypto_crypto_service
    PRIVATE
        $<$<NOT:$<BOOL:${CRYPTO_HW_ACCELERATOR}>>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_mbedcrypto_alt.c>
)

target_compile_options(mbedcrypto_crypto_service
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wno-unused-parameter>
        $<$<C_COMPILER_ID:ARMClang>:-Wno-unused-parameter>
)

target_link_libraries(mbedcrypto_crypto_service
    PRIVATE
        psa_interface
        tfm_secure_api
        platform_s
    PUBLIC
        mbedcrypto_crypto_service_config
)
