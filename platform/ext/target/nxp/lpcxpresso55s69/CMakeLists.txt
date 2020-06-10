#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
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

target_sources(tfm_s
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/startup_LPC55S69_cm33_core0_s.S>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/startup_LPC55S69_cm33_core0_ns.S>
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/LPC55S69_cm33_core0_ns.ld>
    )
    target_link_libraries(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMSIS_5_PATH}/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a>
            # These libraries require linking to the veneers, so the veneers are
            # marked again here as a link dependency to get the ordering right
            tfm_s_veneers
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        Native_Driver
        Native_Driver/project_template/s
        Device/Config
        Device/Include
        partition
        ${PLATFORM_DIR}/ext/common
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str
        ${PLATFORM_DIR}/include
)

target_sources(platform_s
    PRIVATE
        target_cfg.c
        spm_hal.c
        Native_Driver/project_template/s/board.c
        Native_Driver/project_template/s/clock_config.c
        Native_Driver/project_template/s/hardware_init.c
        Native_Driver/project_template/s/peripherals.c
        Native_Driver/project_template/s/pin_mux.c
        Native_Driver/system_LPC55S69_cm33_core0.c
        ../common/CMSIS_Driver/Driver_Flash.c
        ../common/CMSIS_Driver/Driver_USART.c
        ../common/Native_Driver/components/lists/generic_list.c
        ../common/Native_Driver/components/serial_manager/serial_manager.c
        ../common/Native_Driver/components/serial_manager/serial_port_uart.c
        ../common/Native_Driver/components/uart/usart_adapter.c
        ../common/Native_Driver/drivers/fsl_casper.c
        ../common/Native_Driver/drivers/fsl_clock.c
        ../common/Native_Driver/drivers/fsl_common.c
        ../common/Native_Driver/drivers/fsl_ctimer.c
        ../common/Native_Driver/drivers/fsl_flexcomm.c
        ../common/Native_Driver/drivers/fsl_gpio.c
        ../common/Native_Driver/drivers/fsl_hashcrypt.c
        ../common/Native_Driver/drivers/fsl_iap.c
        ../common/Native_Driver/drivers/fsl_power.c
        ../common/Native_Driver/drivers/fsl_reset.c
        ../common/Native_Driver/drivers/fsl_rng.c
        ../common/Native_Driver/drivers/fsl_usart.c
        ../common/Native_Driver/utilities/fsl_assert.c
        ../common/Native_Driver/utilities/fsl_sbrk.c
        ../common/Native_Driver/utilities/debug_console/fsl_debug_console.c
        ../common/Native_Driver/utilities/str/fsl_str.c
        ../common/Native_Driver/mpu_armv8m_drv.c
        ../common/Native_Driver/components/lists/generic_list.c
        ../common/Native_Driver/components/serial_manager/serial_manager.c
        ../common/Native_Driver/components/serial_manager/serial_port_uart.c
        ../common/Native_Driver/components/uart/usart_adapter.c
        ../common/Native_Driver/drivers/fsl_casper.c
        ../common/Native_Driver/drivers/fsl_clock.c
        ../common/Native_Driver/drivers/fsl_common.c
        ../common/Native_Driver/drivers/fsl_ctimer.c
        ../common/Native_Driver/drivers/fsl_flexcomm.c
        ../common/Native_Driver/drivers/fsl_gpio.c
        ../common/Native_Driver/drivers/fsl_hashcrypt.c
        ../common/Native_Driver/drivers/fsl_iap.c
        ../common/Native_Driver/drivers/fsl_power.c
        ../common/Native_Driver/drivers/fsl_reset.c
        ../common/Native_Driver/drivers/fsl_rng.c
        ../common/Native_Driver/drivers/fsl_usart.c
        ../common/Native_Driver/utilities/fsl_assert.c
        ../common/Native_Driver/utilities/fsl_sbrk.c
        ../common/Native_Driver/utilities/debug_console/fsl_debug_console.c
        ../common/Native_Driver/utilities/str/fsl_str.c
        ../lpcxpresso55s69/Native_Driver/system_LPC55S69_cm33_core0.c
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_compile_definitions(platform_s
    PUBLIC
        $<$<C_COMPILER_ID:GNU>:__STARTUP_CLEAR_BSS_MULTIPLE>
        $<$<C_COMPILER_ID:GNU>:__STARTUP_COPY_MULTIPLE>
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        Native_Driver/system_LPC55S69_cm33_core0.c
        Native_Driver/project_template/ns/board.c
        Native_Driver/project_template/ns/hardware_init.c
        Native_Driver/project_template/ns/clock_config.c
        Native_Driver/project_template/ns/peripherals.c
        Native_Driver/project_template/ns/pin_mux.c
        ${PLATFORM_DIR}/ext/target/nxp/common/CMSIS_Driver/Driver_USART.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists/generic_list.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/serial_manager.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/serial_port_uart.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart/usart_adapter.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_casper.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_clock.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_common.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_ctimer.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_flexcomm.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_gpio.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_hashcrypt.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_power.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_reset.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_rng.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_usart.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_assert.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_sbrk.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str/fsl_str.c
)

target_include_directories(platform_ns
    PUBLIC
        .
        Device/Config
        Device/Include
        Native_Driver
        Native_Driver/project_template/ns
        ${PLATFORM_DIR}/ext/common
        ${PLATFORM_DIR}/ext/driver
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str
)
