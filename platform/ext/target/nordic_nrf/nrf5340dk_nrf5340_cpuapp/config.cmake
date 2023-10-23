#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(PLATFORM_PATH platform/ext/target/${TFM_PLATFORM}/..)

# Needed when this config file is included from tf-m-tests/tests_reg
if (DEFINED CONFIG_TFM_SOURCE_PATH)
    set(PLATFORM_PATH ${CONFIG_TFM_SOURCE_PATH}/${PLATFORM_PATH})
endif()

include(${PLATFORM_PATH}/common/nrf5340/config.cmake)
