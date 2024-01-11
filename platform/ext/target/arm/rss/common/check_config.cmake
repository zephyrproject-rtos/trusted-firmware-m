#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# IAR is currently not supported for RSS due to a lack of scatter files
tfm_invalid_config(${CMAKE_C_COMPILER_ID} STREQUAL IAR)

# CPU (Cortex-M55) is only supported in Armclang version 6.14 or newer
tfm_invalid_config(${CMAKE_C_COMPILER_ID} STREQUAL ARMClang AND ${CMAKE_C_COMPILER_VERSION} VERSION_LESS "6.14")

# Only GCC is supported for using ROM library from SRAM
tfm_invalid_config(RSS_USE_ROM_LIB_FROM_SRAM AND NOT ${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

# RSS requires BL1 and BL2
tfm_invalid_config(NOT BL1)
tfm_invalid_config(NOT BL2)
