#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

#========================= Platform common defs ===============================#

if (${CMAKE_C_COMPILER_ID} STREQUAL IAR)
    message(FATAL_ERROR "IAR is currently not supported on the mps3/corstone310_fvp due to a lack of scatter files")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL ARMClang)
    if (${CMAKE_C_COMPILER_VERSION} VERSION_LESS "6.14")
        message(FATAL_ERROR "Architecture (Armv8.1-M) is only supported in ARMCLANG version 6.14 or newer.")
    endif()
endif()

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_corstone310_s.c
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_corstone310_ns.c
    )
    if(DEFAULT_NS_SCATTER)
        target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/corstone310_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/corstone310_ns.ld>
        )
    endif()
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_corstone310_bl2.c
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/corstone310_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/corstone310_bl2.ld>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        libraries
        native_drivers
        partition
        services/src
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        cmsis_drivers/Driver_USART.c
        device/source/device_definition.c
        device/source/system_core_init.c
        native_drivers/mpc_sie_drv.c
        native_drivers/mpu_armv8m_drv.c
        native_drivers/ppc_corstone310_drv.c
        native_drivers/syscounter_armv8-m_cntrl_drv.c
        native_drivers/uart_cmsdk_drv.c
        native_drivers/dma350_ch_drv.c
        native_drivers/dma350_drv.c
        libraries/dma350_lib.c
        spm_hal.c
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_interrupts.c>
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_nvic.c
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/dma350_checker_layer.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
        )

target_compile_definitions(platform_s
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_DEVICE_HEADER=<corstone310.h>

        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/systimer_armv8-m_drv.c>
)

target_link_libraries(platform_s
    PRIVATE
        tfm_utilities
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_USART.c
        device/source/device_definition.c
        device/source/system_core_init.c
        native_drivers/uart_cmsdk_drv.c
)

target_include_directories(platform_ns
    PUBLIC
        .
        ../common
        ${PLATFORM_DIR}/..
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        libraries
        native_drivers
        partition
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            cmsis_drivers/Driver_Flash.c
            cmsis_drivers/Driver_USART.c
            device/source/device_definition.c
            device/source/system_core_init.c
            native_drivers/uart_cmsdk_drv.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            cmsis_drivers
            cmsis_drivers/config
            device
            device/config
            device/include
            device/source/armclang
            libraries
            native_drivers
            partition
            services/src

        PRIVATE
            .
            ${PLATFORM_DIR}/..
            native_drivers
    )
endif()
