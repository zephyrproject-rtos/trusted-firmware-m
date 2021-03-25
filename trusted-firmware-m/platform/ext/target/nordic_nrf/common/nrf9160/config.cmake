#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(SECURE_UART1                        ON         CACHE BOOL      "Enable secure UART1")
set(PSA_API_TEST_TARGET                 "nrf"      CACHE STRING    "PSA API test target")
set(ITS_NUM_ASSETS                      "5"        CACHE STRING    "The maximum number of assets to be stored in the Internal Trusted Storage area")
