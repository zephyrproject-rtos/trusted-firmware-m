#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2021, NXP Semiconductors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#========================= Pull MCUxpresso NXP SDK drivers from https://github.com/NXPmicro/mcux-sdk =========================# 
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/casper/fsl_casper.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_casper.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/casper/fsl_casper.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_casper.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/common/fsl_common.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_common.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/common/fsl_common.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_common.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/ctimer/fsl_ctimer.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_ctimer.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/ctimer/fsl_ctimer.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_ctimer.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/flexcomm/fsl_flexcomm.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_flexcomm.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/flexcomm/fsl_flexcomm.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_flexcomm.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/lpc_gpio/fsl_gpio.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_gpio.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/lpc_gpio/fsl_gpio.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_gpio.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/hashcrypt/fsl_hashcrypt.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_hashcrypt.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/hashcrypt/fsl_hashcrypt.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_hashcrypt.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/iap1/fsl_iap.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/iap1/fsl_iap.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap.h)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/iap1/fsl_iap_kbp.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap_kbp.h)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/iap1/fsl_iap_skboot_authenticate.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap_skboot_authenticate.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/iap1/fsl_iap_ffr.h  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap_ffr.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/lpc_iocon/fsl_iocon.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iocon.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/rng_1/fsl_rng.c  ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_rng.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/rng_1/fsl_rng.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_rng.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/flexcomm/fsl_usart.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_usart.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/drivers/flexcomm/fsl_usart.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_usart.h)

#========================= Pull MCUxpresso NXP SDK components from https://github.com/NXPmicro/mcux-sdk =========================# 
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/lists/fsl_component_generic_list.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists/fsl_component_generic_list.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/lists/fsl_component_generic_list.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists/fsl_component_generic_list.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/serial_manager/fsl_component_serial_manager.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/fsl_component_serial_manager.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/serial_manager/fsl_component_serial_manager.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/fsl_component_serial_manager.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/serial_manager/fsl_component_serial_port_internal.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/fsl_component_serial_port_internal.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/serial_manager/fsl_component_serial_port_uart.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/fsl_component_serial_port_uart.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/serial_manager/fsl_component_serial_port_uart.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/fsl_component_serial_port_uart.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/uart/fsl_adapter_uart.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart/fsl_adapter_uart.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/components/uart/fsl_adapter_usart.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart/fsl_adapter_usart.c)

#========================= Pull MCUxpresso NXP SDK utilities from https://github.com/NXPmicro/mcux-sdk =========================#
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/debug_console/debug_console/fsl_debug_console.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/debug_console/debug_console/fsl_debug_console.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console.h)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/debug_console/debug_console/fsl_debug_console_conf.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console_conf.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/debug_console/str/fsl_str.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str/fsl_str.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/debug_console/str/fsl_str.h ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str/fsl_str.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/utilities/assert/fsl_assert.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_assert.c)


#========================= Pull MCUxpresso NXP SDK devices from https://github.com/NXPmicro/mcux-sdk =========================# 
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/fsl_device_registers.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/fsl_device_registers.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/LPC55S69_cm33_core0.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/LPC55S69_cm33_core0.h)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/LPC55S69_cm33_core0_features.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/LPC55S69_cm33_core0_features.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/LPC55S69_cm33_core1.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/LPC55S69_cm33_core1.h)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/LPC55S69_cm33_core1_features.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/LPC55S69_cm33_core1_features.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/system_LPC55S69_cm33_core0.c ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/system_LPC55S69_cm33_core0.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/system_LPC55S69_cm33_core0.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/system_LPC55S69_cm33_core0.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/utilities/fsl_notifier.c ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/utilities/fsl_notifier.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/utilities/fsl_notifier.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/utilities/fsl_notifier.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/utilities/misc_utilities/fsl_sbrk.c ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_sbrk.c)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/utilities/fsl_shell.c ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/utilities/fsl_shell.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/utilities/fsl_shell.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/utilities/fsl_shell.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_power.c  ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_power.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_power.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_power.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_reset.c  ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_reset.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_reset.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_reset.h)

file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_clock.c  ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_clock.c)
file(DOWNLOAD https://raw.githubusercontent.com/NXPmicro/mcux-sdk/MCUX_2.9.0/devices/LPC55S69/drivers/fsl_clock.h ${PLATFORM_DIR}/ext/target/nxp/lpcxpresso55s69/Native_Driver/drivers/fsl_clock.h)