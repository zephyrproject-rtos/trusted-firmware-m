#
# Copyright (c) 2023, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# This file is exported to NS side during CMake installation phase and renamed
# to CMakeLists.txt. It instructs how to build a platform on non-secture side.
# The structure and sources list are fully platform specific.

include(config_nordic_nrf_spe.cmake)

# Startup sources should be put in the executable
target_sources(tfm_ns
    PRIVATE
        startup.c
)

if (TFM_PARTITION_PLATFORM)
    # Additional NS API sources
    target_sources(tfm_api_ns
        PRIVATE
            ${CONFIG_SPE_PATH}/interface/src/tfm_ioctl_core_ns_api.c
)
endif()

add_library(platform_ns)

add_subdirectory(soc)

target_sources(platform_ns
    PRIVATE
        cmsis_drivers/Driver_USART.c
        ${HAL_NORDIC_PATH}/nrfx/drivers/src/nrfx_uarte.c
        nrfx_glue.c
        uart_stdout.c
        $<$<BOOL:${TEST_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/pal_plat_test.c>
)

target_compile_definitions(platform_ns
    PUBLIC
        NRF_TRUSTZONE_NONSECURE
        DOMAIN_NS=1
)

target_include_directories(platform_ns
    PUBLIC
        include
        common
        cmsis
        cmsis_drivers
        ${HAL_NORDIC_PATH}/nrfx
        ${HAL_NORDIC_PATH}/nrfx/mdk
        ${HAL_NORDIC_PATH}/nrfx/drivers/include

)

target_compile_definitions(platform_ns
    PUBLIC
        $<$<BOOL:${PLATFORM_DEFAULT_CRYPTO_KEYS}>:PLATFORM_DEFAULT_CRYPTO_KEYS>
        $<$<BOOL:${TEST_PSA_API}>:PSA_API_TEST_ENABLED>
)

target_compile_definitions(platform_ns
    PUBLIC
        BL2=ON
        BL2_HEADER_SIZE=${BL2_HEADER_SIZE}
        BL2_TRAILER_SIZE=${BL2_TRAILER_SIZE}
)
