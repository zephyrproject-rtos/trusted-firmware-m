#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

add_library(platform_s STATIC)
add_library(platform_region_defs INTERFACE)

# under the multicore topology, NS is declared in the same place as the PSA
# interface so that it picks up the compiler definitions for the NS CPU
if(NOT TFM_MULTI_CORE_TOPOLOGY)
    add_library(platform_ns STATIC)
endif()

if (BL2)
    add_library(platform_bl2 STATIC)
endif()

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(ext/target/${TFM_PLATFORM})

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/driver>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/cmsis>
        $<$<BOOL:${CRYPTO_HW_ACCELERATOR}>:${CMAKE_CURRENT_SOURCE_DIR}/ext/accelerator/interface>
)

target_sources(platform_s
    PRIVATE
        ext/common/template/attest_hal.c
        ext/common/tfm_hal_ps.c
        ext/common/tfm_hal_its.c
        ext/common/tfm_platform.c
        ext/common/uart_stdout.c
        ext/common/tfm_hal_spm_logdev_peripheral.c
        $<$<BOOL:${PLATFORM_DUMMY_ATTEST_HAL}>:ext/common/template/attest_hal.c>
        $<$<BOOL:${PLATFORM_DUMMY_NV_COUNTERS}>:ext/common/template/nv_counters.c>
        $<$<BOOL:${PLATFORM_DUMMY_CRYPTO_KEYS}>:ext/common/template/crypto_keys.c>
        $<$<BOOL:${PLATFORM_DUMMY_ROTPK}>:ext/common/template/tfm_rotpk.c>
        $<$<BOOL:${PLATFORM_DUMMY_IAK}>:ext/common/template/tfm_initial_attestation_key_material.c>
)

target_link_libraries(platform_s
    PUBLIC
        platform_region_defs
    PRIVATE
        psa_interface
        tfm_secure_api
        tfm_arch
)

target_compile_definitions(platform_s
    PUBLIC
        TFM_SPM_LOG_LEVEL=${TFM_SPM_LOG_LEVEL}
    PRIVATE
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:SYMMETRIC_INITIAL_ATTESTATION>
)

#========================= Platform Non-Secure ================================#

target_include_directories(platform_ns
    PUBLIC
        ext
        ext/common
        ext/driver
        ext/common
        include
        ext/cmsis
)

target_sources(platform_ns
    PRIVATE
        ext/common/uart_stdout.c
)

target_link_libraries(platform_ns
    PUBLIC
        platform_region_defs
        psa_interface
)

target_compile_definitions(platform_ns
    PRIVATE
        $<$<BOOL:${TFM_IRQ_TEST}>:TFM_ENABLE_IRQ_TEST>
        $<$<BOOL:${TFM_PERIPH_ACCESS_TEST}>:TFM_ENABLE_PERIPH_ACCESS_TEST>
        $<$<BOOL:${TFM_MULTI_CORE_MULTI_CLIENT_CALL}>:TFM_MULTI_CORE_MULTI_CLIENT_CALL>
)

#========================= Platform BL2 =======================================#
if(BL2)
    #TODO import policy
    target_include_directories(platform_bl2
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/cmsis>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/driver>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/common>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext>
            $<$<BOOL:${CRYPTO_HW_ACCELERATOR}>:${CMAKE_CURRENT_SOURCE_DIR}/ext/accelerator/interface>
    )

    target_sources(platform_bl2
        PRIVATE
            ext/common/uart_stdout.c
            ext/common/boot_hal.c
            $<$<BOOL:${PLATFORM_DUMMY_NV_COUNTERS}>:ext/common/template/nv_counters.c>
            $<$<BOOL:${PLATFORM_DUMMY_ROTPK}>:ext/common/template/tfm_rotpk.c>
            $<$<BOOL:${PLATFORM_DUMMY_IAK}>:ext/common/template/tfm_initial_attestation_key_material.c>
    )

    target_link_libraries(platform_bl2
        PUBLIC
            platform_region_defs
        PRIVATE
            psa_interface
            bl2_hal
            tfm_arch
    )

    if (NOT DEFINED MCUBOOT_UPGRADE_STRATEGY)
        message(FATAL_ERROR "MCUBOOT_UPGRADE_STRATEGY is undefined")
    else()
        string(CONCAT UPGRADE_STRATEGY "MCUBOOT_" ${MCUBOOT_UPGRADE_STRATEGY})
    endif()

    target_compile_definitions(platform_bl2
        PUBLIC
            BL2
            MCUBOOT_${MCUBOOT_UPGRADE_STRATEGY}
            $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:SYMMETRIC_INITIAL_ATTESTATION>
            $<$<BOOL:${MCUBOOT_HW_KEY}>:MCUBOOT_HW_KEY>
            MCUBOOT_FIH_PROFILE_${MCUBOOT_FIH_PROFILE}
        )
endif()

#========================= Platform region defs ===============================#

#TODO maybe just link the other platforms to this
target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${BL2}>:BL2>
        $<$<BOOL:${SECURE_UART1}>:SECURE_UART1>
        DAUTH_${DEBUG_AUTHENTICATION}
        MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}
        $<$<STREQUAL:${MCUBOOT_SIGNATURE_TYPE},RSA>:MCUBOOT_SIGN_RSA>
        $<$<STREQUAL:${MCUBOOT_SIGNATURE_TYPE},RSA>:MCUBOOT_SIGN_RSA_LEN=${MCUBOOT_SIGNATURE_KEY_LEN}>
        $<$<STREQUAL:${MCUBOOT_EXECUTION_SLOT},2>:LINK_TO_SECONDARY_PARTITION>
        $<$<BOOL:${TEST_PSA_API}>:PSA_API_TEST_${TEST_PSA_API}>
)
