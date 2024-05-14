#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
# Copyright (c) 2024 ArmChina. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(${ALCOR_COMMON_DIR}/config.cmake)

set(PLATFORM_DEFAULT_SYSTEM_RESET_HALT OFF   CACHE BOOL    "Use default system reset/halt implementation")

set(PROVISIONING_CODE_PADDED_SIZE       "0x2400"  CACHE STRING    "")
set(PROVISIONING_VALUES_PADDED_SIZE     "0x400"   CACHE STRING    "")
set(PROVISIONING_DATA_PADDED_SIZE       "0x400"   CACHE STRING    "")
