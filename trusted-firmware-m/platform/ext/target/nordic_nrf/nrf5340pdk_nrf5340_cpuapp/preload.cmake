#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(NRF_PATH platform/ext/target/${TFM_PLATFORM}/..)

include(${NRF_PATH}/common/nrf5340/preload.cmake)

add_definitions(-DNRF5340_ENG_A)