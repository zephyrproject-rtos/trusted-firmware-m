#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

install(FILES ${PLATFORM_DIR}/ext/common/uart_stdout.c
              ${TARGET_PLATFORM_PATH}/native_drivers/arm_uart_drv.c
              ${TARGET_PLATFORM_PATH}/native_drivers/timer_cmsdk/timer_cmsdk.c
              ${TARGET_PLATFORM_PATH}/cmsis_drivers/Driver_USART.c
              ${TARGET_PLATFORM_PATH}/retarget/platform_retarget_dev.c
              ${TARGET_PLATFORM_PATH}/cmsis_core/an521_ns_init.c
              DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(DIRECTORY ${TARGET_PLATFORM_PATH}/cmsis_core
              DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(DIRECTORY ${TARGET_PLATFORM_PATH}/retarget
              DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(DIRECTORY ${PLATFORM_DIR}/ext/common
              DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)

install(DIRECTORY ${PLATFORM_DIR}/ext/driver
              DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)

install(FILES ${TARGET_PLATFORM_PATH}/partition/region_defs.h
              ${TARGET_PLATFORM_PATH}/partition/flash_layout.h
              ${TARGET_PLATFORM_PATH}/native_drivers/arm_uart_drv.h
              ${TARGET_PLATFORM_PATH}/native_drivers/timer_cmsdk/timer_cmsdk.h
              ${TARGET_PLATFORM_PATH}/native_drivers/mpc_sie200_drv.h
              ${TARGET_PLATFORM_PATH}/native_drivers/ppc_sse200_drv.h
              ${TARGET_PLATFORM_PATH}/retarget/platform_retarget.h
              ${TARGET_PLATFORM_PATH}/retarget/platform_retarget_pins.h
              ${TARGET_PLATFORM_PATH}/target_cfg.h
              ${TARGET_PLATFORM_PATH}/device_cfg.h
              ${TARGET_PLATFORM_PATH}/tfm_peripherals_def.h
              ${TARGET_PLATFORM_PATH}/cmsis_driver_config.h
              ${TARGET_PLATFORM_PATH}/RTE_Device.h
              ${PLATFORM_DIR}/ext/common/uart_stdout.h
              ${PLATFORM_DIR}/ext/driver/Driver_USART.h
              ${PLATFORM_DIR}/ext/driver/Driver_Common.h
              ${PLATFORM_DIR}/include/tfm_plat_defs.h
              ${CMAKE_SOURCE_DIR}/lib/fih/inc/fih.h
              DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)
