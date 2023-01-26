#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        ${CORSTONE310_COMMON_DIR}/partition
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    ${CORSTONE310_COMMON_DIR}/device/source/startup_corstone310.c
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CORSTONE310_COMMON_DIR}/device/source/startup_corstone310.c
    )
    if(DEFAULT_NS_SCATTER)
        target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld>
            $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_ns.icf>
        )
    endif()
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            $<$<C_COMPILER_ID:ARMClang>:CMSIS_5_RTX_V8MMN>
            $<$<C_COMPILER_ID:GNU>:CMSIS_5_RTX_V8MMN>
            $<$<C_COMPILER_ID:IAR>:CMSIS_5_RTX_V81MMN>
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            ${CORSTONE310_COMMON_DIR}/device/source/startup_corstone310.c
    )
    target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_bl2.ld>
        $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_bl2.icf>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        ${CORSTONE310_COMMON_DIR}
        ../common
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/secure
        ${CORSTONE310_COMMON_DIR}/device
        ${CORSTONE310_COMMON_DIR}/device/include
        ${CORSTONE310_COMMON_DIR}/device/source/armclang
        ${CORSTONE310_COMMON_DIR}/native_drivers
        ${CORSTONE310_COMMON_DIR}/partition
        ${CORSTONE310_COMMON_DIR}/services/src
        ${PLATFORM_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
)

target_sources(platform_s
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_Flash.c
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_MPC.c
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_TGU.c
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_PPC.c
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_USART.c
        ${CORSTONE310_COMMON_DIR}/device/source/platform_s_device_definition.c
        ${CORSTONE310_COMMON_DIR}/device/source/system_core_init.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/mpc_sie_drv.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/mpu_armv8m_drv.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/ppc_corstone310_drv.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/syscounter_armv8-m_cntrl_drv.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/uart_cmsdk_drv.c
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CORSTONE310_COMMON_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CORSTONE310_COMMON_DIR}/services/src/tfm_platform_system.c>
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CORSTONE310_COMMON_DIR}/native_drivers/systimer_armv8-m_drv.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_Flash.c
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_USART.c
        ${CORSTONE310_COMMON_DIR}/device/source/platform_ns_device_definition.c
        ${CORSTONE310_COMMON_DIR}/device/source/system_core_init.c
        ${CORSTONE310_COMMON_DIR}/native_drivers/uart_cmsdk_drv.c
)

target_include_directories(platform_ns
    PUBLIC
        ${CORSTONE310_COMMON_DIR}
        ../common
        ${PLATFORM_DIR}/..
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/non_secure
        ${CORSTONE310_COMMON_DIR}/device
        ${CORSTONE310_COMMON_DIR}/device/include
        ${CORSTONE310_COMMON_DIR}/device/source/armclang
        ${CORSTONE310_COMMON_DIR}/native_drivers
        ${CORSTONE310_COMMON_DIR}/partition
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_Flash.c
            ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_USART.c
            ${CORSTONE310_COMMON_DIR}/device/source/platform_s_device_definition.c
            ${CORSTONE310_COMMON_DIR}/device/source/system_core_init.c
            ${CORSTONE310_COMMON_DIR}/native_drivers/uart_cmsdk_drv.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            ${CORSTONE310_COMMON_DIR}/cmsis_drivers
            ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/secure
            ${CORSTONE310_COMMON_DIR}/device
            ${CORSTONE310_COMMON_DIR}/device/include
            ${CORSTONE310_COMMON_DIR}/device/source/armclang
            ${CORSTONE310_COMMON_DIR}/native_drivers
            ${CORSTONE310_COMMON_DIR}/partition
            ${CORSTONE310_COMMON_DIR}/services/src
            ${CMAKE_CURRENT_SOURCE_DIR}/device/config

        PRIVATE
            ${CORSTONE310_COMMON_DIR}
            ${PLATFORM_DIR}/..
            ${CORSTONE310_COMMON_DIR}/native_drivers
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/target_cfg.c
        ${CORSTONE310_COMMON_DIR}/tfm_hal_isolation.c
        ${CORSTONE310_COMMON_DIR}/tfm_hal_platform.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dma_init.c
        $<$<OR:$<BOOL:${CONFIG_TFM_FLIH_API}>,$<BOOL:${CONFIG_TFM_SLIH_API}>>:${CORSTONE310_COMMON_DIR}/tfm_interrupts.c>
)
