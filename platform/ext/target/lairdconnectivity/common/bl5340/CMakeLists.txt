#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2020, Nordic Semiconductor ASA.
# Copyright (c) 2021, Laird Connectivity.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(../core nrf_common)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}../${NRF_FOLDER_PATH}/../nrf5340/gcc/startup_nrf5340_s.S>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}../${NRF_FOLDER_PATH}/../nrf5340/gcc/startup_nrf5340_ns.S>
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}../${NRF_FOLDER_PATH}/../nrf5340/gcc/startup_nrf5340_bl2.S>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
)

target_sources(platform_s
    PRIVATE
        target_cfg.c
        ${NRF_FOLDER_PATH}/nrfx/mdk/system_nrf5340_application.c
)

target_compile_definitions(platform_s
    PUBLIC
        NRF5340_XXAA_APPLICATION
        $<$<BOOL:${SECURE_QSPI}>:SECURE_QSPI>
)

#========================= Platform Non-Secure ================================#

target_include_directories(platform_ns
    PUBLIC
        .
)

target_sources(platform_ns
    PRIVATE
        ${NRF_FOLDER_PATH}/nrfx/mdk/system_nrf5340_application.c
)

target_compile_definitions(platform_ns
    PUBLIC
        NRF5340_XXAA_APPLICATION
        NRF_TRUSTZONE_NONSECURE
        DOMAIN_NS=1
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_include_directories(platform_bl2
        PRIVATE
            .
    )

    target_sources(platform_bl2
        PRIVATE
            ${NRF_FOLDER_PATH}/nrfx/mdk/system_nrf5340_application.c
    )

    target_compile_definitions(platform_bl2
        PUBLIC
            NRF5340_XXAA_APPLICATION
    )
endif()

#========================= Platform region defs ===============================#

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${NRF_NS_STORAGE}>:NRF_NS_STORAGE>
)
