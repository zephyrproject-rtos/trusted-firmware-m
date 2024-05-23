#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(common.cmake)

target_sources(platform_ns
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_USART.c
        device/host_device_definition.c
        device/rse_expansion_device_definition.c
        native_drivers/cfi_drv.c
        native_drivers/spi_strataflashj3_flash_lib.c
        native_drivers/uart_pl011_drv.c
)
