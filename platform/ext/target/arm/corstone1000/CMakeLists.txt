#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
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
        cc312
)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${TFM_S_REG_TEST}>:TFM_S_REG_TEST>
)
#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_corstone1000.c>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
)

target_sources(bl2
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/startup_corstone1000.c>
)
target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_bl2.ld>
)

#========================= Platform Secure ====================================#

add_subdirectory(openamp)

target_include_directories(platform_s
    PUBLIC
        .
        CMSIS_Driver
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
        partition
        services/include
    INTERFACE
        cc312
        fw_update_agent
)

target_sources(platform_s
    PRIVATE
        CMSIS_Driver/Driver_Flash.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/uart_pl011_drv.c
        Native_Driver/mhu_v2_x.c
        Native_Driver/watchdog.c
        Native_Driver/arm_watchdog_drv.c
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        fw_update_agent/uefi_capsule_parser.c
        fw_update_agent/fwu_agent.c
        fw_update_agent/uefi_fmp.c
        $<$<NOT:$<BOOL:${PLATFORM_DEFAULT_OTP}>>:${PLATFORM_DIR}/ext/accelerator/cc312/otp_cc312.c>
)

if (PLATFORM_IS_FVP)
target_sources(platform_s
    PRIVATE
        Native_Driver/spi_strataflashj3_flash_lib.c
        Native_Driver/cfi_drv.c
)
else()
target_sources(platform_s
    PRIVATE
        Native_Driver/xilinx_pg153_axi_qspi_controller_drv.c
        Native_Driver/spi_n25q256a_flash_lib.c
        Native_Driver/spi_sst26vf064b_flash_lib.c
)
endif()

target_compile_definitions(platform_s
    PUBLIC
        $<$<BOOL:${EXTERNAL_SYSTEM_SUPPORT}>:EXTERNAL_SYSTEM_SUPPORT>
        $<$<BOOL:${PLATFORM_IS_FVP}>:PLATFORM_IS_FVP>
    PRIVATE
        $<$<BOOL:${TFM_S_REG_TEST}>:TFM_S_REG_TEST>
        $<$<BOOL:${ENABLE_FWU_AGENT_DEBUG_LOGS}>:ENABLE_FWU_AGENT_DEBUG_LOGS>
)

if (TFM_PARTITION_CRYPTO)
    target_include_directories(platform_crypto_keys
        PUBLIC
            .
    )
endif()

#========================= Platform BL2 =======================================#

set(BL2_SOURCE ${CMAKE_SOURCE_DIR}/bl2)

target_sources(platform_bl2
    PRIVATE
        CMSIS_Driver/Driver_Flash.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/uart_pl011_drv.c
        Native_Driver/watchdog.c
        Native_Driver/arm_watchdog_drv.c
        fip_parser/fip_parser.c
        fw_update_agent/fwu_agent.c
        bl2_security_cnt.c
        $<$<NOT:$<BOOL:${PLATFORM_DEFAULT_OTP}>>:${PLATFORM_DIR}/ext/accelerator/cc312/otp_cc312.c>
)

if (PLATFORM_IS_FVP)
target_sources(platform_bl2
    PRIVATE
        Native_Driver/spi_strataflashj3_flash_lib.c
        Native_Driver/cfi_drv.c
)
else()
target_sources(platform_bl2
    PRIVATE
        Native_Driver/xilinx_pg153_axi_qspi_controller_drv.c
        Native_Driver/spi_n25q256a_flash_lib.c
        Native_Driver/spi_sst26vf064b_flash_lib.c
)
endif()

target_compile_definitions(platform_bl2
    PRIVATE
        $<$<BOOL:${PLATFORM_IS_FVP}>:PLATFORM_IS_FVP>
        $<$<BOOL:${TFM_S_REG_TEST}>:TFM_S_REG_TEST>
        $<$<BOOL:${ENABLE_FWU_AGENT_DEBUG_LOGS}>:ENABLE_FWU_AGENT_DEBUG_LOGS>
)

# boot_hal_bl2.c is compiled as part of 'bl2' target and not inside
# 'platform_bl2' where it suppose to be. This is due to
# platform_init/quit* apis symbol collision in bl1.
target_sources(bl2
    PRIVATE
        bl2_flash_map.c
        boot_hal_bl2.c
)

target_link_libraries(bl2
    PRIVATE
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:trusted-firmware-m-psa-adac>
)

target_compile_definitions(bl2
    PRIVATE
        $<$<BOOL:${CRYPTO_HW_ACCELERATOR}>:CRYPTO_HW_ACCELERATOR>
        $<$<BOOL:${CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING}>:CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING>
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:PLATFORM_PSA_ADAC_SECURE_DEBUG>

)
target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_include_directories(platform_bl2
    PUBLIC
        partition
        Device/Include
        fip_parser
        Native_Driver
        fw_update_agent
        .
    INTERFACE
        cc312
    PRIVATE
        CMSIS_Driver/Config
        Device/Config
        ${MCUBOOT_PATH}/boot/bootutil/include # for fault_injection_hardening.h only
        ${CMAKE_BINARY_DIR}/bl2/ext/mcuboot # for mcuboot_config.h only
        $<BUILD_INTERFACE:${BL2_SOURCE}/ext/mcuboot/include>
)

#========================= BL1 component =======================================#

add_subdirectory(bl1)

#========================= ns_agent_mailbox ===================================#

target_sources(tfm_psa_rot_partition_ns_agent_mailbox
    PRIVATE
        tfm_hal_multi_core.c
)

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        tfm_hal_isolation.c
        tfm_hal_platform.c
        $<$<BOOL:${TFM_S_REG_TEST}>:${CMAKE_CURRENT_SOURCE_DIR}/target_cfg.c>
)

#========================= tfm_adac ============================================#

if (${PLATFORM_PSA_ADAC_SECURE_DEBUG})
    target_compile_definitions(platform_bl2
        PRIVATE
            PLATFORM_PSA_ADAC_SECURE_DEBUG
    )

    target_link_libraries(trusted-firmware-m-psa-adac
        PRIVATE
            platform_bl2
    )
endif()
