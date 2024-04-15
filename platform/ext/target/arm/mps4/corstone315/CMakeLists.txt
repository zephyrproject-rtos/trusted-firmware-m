#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        device/source/startup_corstone315.c
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_s.icf>
)

if(BL2)
    target_sources(bl2
        PRIVATE
            device/source/startup_corstone315.c
    )
    target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_bl2.ld>
        $<$<C_COMPILER_ID:IAR>:${PLATFORM_DIR}/ext/common/iar/tfm_common_bl2.icf>
    )

    target_compile_options(bl2_scatter
        PUBLIC
            ${COMPILER_CMSE_FLAG}
    )
endif()

#========================= Device definition lib ===============================#

add_library(device_definition INTERFACE)
target_include_directories(device_definition
    INTERFACE
        device/include
        native_drivers
        partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/sam
        ${PLATFORM_DIR}/ext/target/arm/drivers/watchdog
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu
        ${PLATFORM_DIR}/ext/target/arm/drivers/tgu
        ${PLATFORM_DIR}/include
        ${ETHOS_DRIVER_PATH}/src
        ${ETHOS_DRIVER_PATH}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
        ${CMAKE_SOURCE_DIR}
)

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        cmsis_drivers/config/secure
        cmsis_drivers
        device
        services/src
        ${PLATFORM_DIR}/ext/common
)

target_sources(platform_s
    PRIVATE
        tfm_peripherals_def.c
        attest_hal.c
        otp_lcm.c
        nv_counters.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_TGU.c
        cmsis_drivers/Driver_PPC.c
        cmsis_drivers/Driver_USART.c
        device/source/platform_s_device_definition.c
        device/source/system_core_init.c
        native_drivers/ppc_corstone315_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/tgu/tgu_armv8_m_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
        ${PLATFORM_DIR}/ext/common/tfm_hal_reset_halt.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie/mpc_sie_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m/syscounter_armv8-m_cntrl_drv.c
        ${ETHOS_DRIVER_PATH}/src/ethosu_device_u55_u65.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        $<$<OR:$<BOOL:${TFM_PARTITION_SLIH_TEST}>,$<BOOL:${TFM_PARTITION_FLIH_TEST}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>

        # DMA350 files
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_ch_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_lib.c
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_checker_layer.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_address_remap.c>
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TFM_PARTITION_SLIH_TEST}>,$<BOOL:${TFM_PARTITION_FLIH_TEST}>>:${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c>
)

# To configure S and NS timer in S side for FP interrupt test
target_compile_definitions(platform_s
    PUBLIC
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
        $<$<BOOL:${TEST_S_FPU}>:TEST_S_FPU>
)

target_compile_definitions(platform_s
    PUBLIC
        ETHOSU_ARCH=$<LOWER_CASE:${ETHOSU_ARCH}>
        ETHOS$<UPPER_CASE:${ETHOSU_ARCH}>
        ETHOSU_LOG_SEVERITY=${ETHOSU_LOG_SEVERITY}
)

target_compile_definitions(platform_s
    INTERFACE
        ATTEST_KEY_BITS=${ATTEST_KEY_BITS}
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header=<corstone315.h>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CP_FLAG}
        ${COMPILER_CMSE_FLAG}
)

target_link_options(platform_s
    PUBLIC
        ${LINKER_CP_OPTION}
)

target_link_libraries(platform_s
    PUBLIC
        device_definition
    PRIVATE
        tfm_sprt # For tfm_strnlen in attest HAL
)

#========================= Platform BL2 =======================================#

if(BL2)
    # boot_hal_bl2.c is compiled as part of 'bl2' target and not inside
    # 'platform_bl2' where it suppose to be. This is due to
    # platform_init/quit* apis symbol collision in bl1.
    target_sources(bl2
        PRIVATE
            bl2/boot_hal_bl2.c
    )

    target_sources(platform_bl2
        PRIVATE
            nv_counters.c
            otp_lcm.c
            ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
            ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
            cmsis_drivers/Driver_USART.c
            device/source/platform_s_device_definition.c
            device/source/system_core_init.c
            ${PLATFORM_DIR}/ext/common/tfm_hal_reset_halt.c
            ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated/emulated_flash_drv.c
            ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            device/include
        PRIVATE
            .
            device
            services/src
            ${CMAKE_CURRENT_SOURCE_DIR}/device/config
            cmsis_drivers/config/secure
    )

    target_compile_definitions(platform_bl2
        PUBLIC
            $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
    )

    target_compile_options(platform_bl2
        PUBLIC
            ${BL2_COMPILER_CP_FLAG}
            ${COMPILER_CMSE_FLAG}
    )

    target_link_options(platform_bl2
        PUBLIC
            ${BL2_LINKER_CP_OPTION}
    )

    target_link_libraries(platform_bl2
        PRIVATE
            $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:tfm_boot_status>
            device_definition
    )

endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        tfm_hal_platform.c
        ${PLATFORM_DIR}/ext/common/tfm_hal_platform_v8m.c
        ${PLATFORM_DIR}/ext/common/tfm_hal_isolation_v8m.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dma_init.c
        $<$<OR:$<BOOL:${CONFIG_TFM_FLIH_API}>,$<BOOL:${CONFIG_TFM_SLIH_API}>>:${PLATFORM_DIR}/ext/common/tfm_interrupts.c>
)

# If this is not added to the tfm_s it will not correctly override the weak
# default handlers declared in assemebly, and will instead be discarded as they
# are not in use.
target_sources(tfm_s
    PRIVATE
        ${PLATFORM_DIR}/ext/common/mpc_ppc_faults.c
)

#========================= platform_region_defs ===============================#
target_compile_definitions(platform_region_defs
    INTERFACE
        S_DATA_OVERALL_SIZE=${S_DATA_OVERALL_SIZE}
        FLASH_S_PARTITION_SIZE=${FLASH_S_PARTITION_SIZE}
        FLASH_NS_PARTITION_SIZE=${FLASH_NS_PARTITION_SIZE}
        PROVISIONING_CODE_PADDED_SIZE=${PROVISIONING_CODE_PADDED_SIZE}
        PROVISIONING_VALUES_PADDED_SIZE=${PROVISIONING_VALUES_PADDED_SIZE}
        PROVISIONING_DATA_PADDED_SIZE=${PROVISIONING_DATA_PADDED_SIZE}
        $<$<BOOL:${PLATFORM_HAS_PS_NV_OTP_COUNTERS}>:PLATFORM_HAS_PS_NV_OTP_COUNTERS>
)

target_include_directories(platform_region_defs
    INTERFACE
        partition
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#========================= Flash driver library ===============================#
add_library(flash_drivers_s STATIC)
add_library(flash_drivers_bl2 STATIC)
add_library(flash_drivers INTERFACE)

target_include_directories(flash_drivers
    INTERFACE
        cmsis_drivers/config/secure
        ${PLATFORM_DIR}/ext/driver
        partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated
)

target_sources(flash_drivers
    INTERFACE
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated/emulated_flash_drv.c
)

include(libflash_drivers.cmake)

#========================= Platform BL1_1 =======================================#

target_add_scatter_file(bl1_1
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/corstone315_bl1_1.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/corstone315_bl1_1.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/corstone315_bl1_1.icf>
)

target_compile_options(bl1_1_scatter
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_compile_definitions(bl1_1
    PRIVATE
        MBEDTLS_CONFIG_FILE="${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/tfm_mbedcrypto_config_default.h"
        MBEDTLS_PSA_CRYPTO_CONFIG_FILE="${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/crypto_config_default.h"
)

target_compile_options(bl1_1
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_sources(platform_bl1_1
    PRIVATE
        nv_counters.c
        otp_lcm.c
        cmsis_drivers/Driver_USART.c
        device/source/platform_s_device_definition.c
        device/source/system_core_init.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
        ${PLATFORM_DIR}/ext/common/tfm_hal_reset_halt.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/watchdog/arm_watchdog_drv.c
)

target_include_directories(platform_bl1_1
    PRIVATE
        cmsis_drivers/config/secure
)

target_compile_options(platform_bl1_1
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_link_libraries(platform_bl1_1
    PUBLIC
        device_definition
        flash_drivers_bl2
    PRIVATE
        bl1_1_shared_lib_interface
)

# If this is not added to the bl1_1 it will not correctly override the weak
# functions and will instead be discarded as they are not in use.
target_sources(bl1_1
    PUBLIC
        bl1/boot_hal_bl1_1.c
        device/source/startup_corstone315.c
)

#========================= Platform BL1_2 =====================================#

target_add_scatter_file(bl1_2
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/corstone315_bl1_2.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/corstone315_bl1_2.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/corstone315_bl1_2.icf>
)

target_compile_definitions(bl1_2
    PRIVATE
        MBEDTLS_CONFIG_FILE="${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/tfm_mbedcrypto_config_default.h"
        MBEDTLS_PSA_CRYPTO_CONFIG_FILE="${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto/mbedcrypto_config/crypto_config_default.h"
)

target_compile_options(bl1_2_scatter
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_compile_options(bl1_2
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_sources(platform_bl1_2
    PRIVATE
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c

)

target_include_directories(platform_bl1_2
    PRIVATE
        cmsis_drivers/config/secure
)

target_compile_options(platform_bl1_2
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

# If this is not added to the bl1_2 it will not correctly override the weak
# functions and will instead be discarded as they are not in use.
target_sources(bl1_2
    PRIVATE
        bl1/boot_hal_bl1_2.c
        device/source/startup_corstone315.c
)

target_compile_definitions(platform_bl1_2
    PUBLIC
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
)

target_link_libraries(platform_bl1_2
    PUBLIC
        device_definition
)

#========================= Platform Crypto Keys ===============================#

if (TFM_PARTITION_CRYPTO)
    target_sources(platform_crypto_keys
        PRIVATE
            crypto_keys.c
    )
    target_link_libraries(platform_crypto_keys
        PRIVATE
            platform_s
    )
endif()

#========================= Provisioning =======================================#
add_subdirectory(${TARGET_PLATFORM_PATH}/provisioning provisioning)

#========================= Files for building NS side platform ================#
target_compile_definitions(tfm_config
    INTERFACE
        S_DATA_OVERALL_SIZE=${S_DATA_OVERALL_SIZE}
        FLASH_S_PARTITION_SIZE=${FLASH_S_PARTITION_SIZE}
        FLASH_NS_PARTITION_SIZE=${FLASH_NS_PARTITION_SIZE}
        PROVISIONING_CODE_PADDED_SIZE=${PROVISIONING_CODE_PADDED_SIZE}
        PROVISIONING_VALUES_PADDED_SIZE=${PROVISIONING_VALUES_PADDED_SIZE}
        PROVISIONING_DATA_PADDED_SIZE=${PROVISIONING_DATA_PADDED_SIZE}
)

install(FILES       ${TARGET_PLATFORM_PATH}/ns/cpuarch_ns.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
        RENAME      cpuarch.cmake)

install(FILES       cmsis_drivers/Driver_USART.c
                    cmsis_drivers/config/non_secure/cmsis_driver_config.h
                    cmsis_drivers/config/non_secure/RTE_Device.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/cmsis_drivers)

install(FILES       ${PLATFORM_DIR}/ext/common/common_target_cfg.h
                    ${PLATFORM_DIR}/ext/common/test_interrupt.h
                    ${PLATFORM_DIR}/ext/common/test_interrupt.c
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/ext/common)

install(DIRECTORY   device
                    native_drivers
                    cmsis_drivers
                    partition
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(DIRECTORY   ${PLATFORM_DIR}/ext/target/arm/drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/ext/target/arm)

install(FILES       ${PLATFORM_DIR}/ext/driver/Driver_USART.h
                    ${PLATFORM_DIR}/ext/driver/Driver_Common.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/ext/driver)

install(FILES       target_cfg.h
                    tfm_peripherals_def.h
                    ${PLATFORM_DIR}/include/tfm_plat_defs.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)

install(FILES       cpuarch.cmake
                    config.cmake
                    check_config.cmake
                    target_cfg.h
                    tfm_builtin_key_ids.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       tfm_builtin_key_ids.h
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})

install(DIRECTORY   tests
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(DIRECTORY   ${TARGET_PLATFORM_PATH}/ns/
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

if(DEFAULT_NS_SCATTER)
    # Install linker scripts
    install(FILES       ${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct
                        ${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld
                        ${PLATFORM_DIR}/ext/common/iar/tfm_common_ns.icf
            DESTINATION ${INSTALL_PLATFORM_NS_DIR}/linker_scripts)
endif()
