#-------------------------------------------------------------------------------
# Copyright (c) 2024, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
#-------------------------------------------------------------------------------

include(${PLATFORM_PATH}/common/nrf54l15/config.cmake)

set(SECURE_UART1                        OFF         CACHE BOOL      "Enable secure UART1" FORCE)
set(SECURE_UART30                       OFF         CACHE BOOL      "Enable secure UART1" FORCE)
set(BL2                                 OFF         CACHE BOOL      "Enable secure UART1" FORCE)