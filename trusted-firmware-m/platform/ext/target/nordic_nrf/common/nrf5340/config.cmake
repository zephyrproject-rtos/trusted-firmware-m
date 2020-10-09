#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(SECURE_UART1                        ON         CACHE BOOL      "Enable secure UART1" FORCE)
set(PSA_API_TEST_TARGET                 "nrf"      CACHE STRING    "PSA API test target" FORCE)
set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/${NRF_PATH}/common/core/generated_file_list.yaml          CACHE PATH      "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)
