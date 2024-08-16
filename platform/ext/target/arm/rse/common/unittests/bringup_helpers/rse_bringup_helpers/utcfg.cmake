#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(PLATFORM_DIR ${TFM_ROOT_DIR}/platform)
set(RSE_COMMON_SOURCE_DIR ${PLATFORM_DIR}/ext/target/arm/rse/common)

#-------------------------------------------------------------------------------
# Unit under test
#-------------------------------------------------------------------------------
set(UNIT_UNDER_TEST ${RSE_COMMON_SOURCE_DIR}/bringup_helpers/rse_bringup_helpers.c)

#-------------------------------------------------------------------------------
# Test suite
#-------------------------------------------------------------------------------
set(UNIT_TEST_SUITE ${CMAKE_CURRENT_LIST_DIR}/test_rse_bringup_helpers.c)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/unittests/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/bringup_helpers)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/device/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/partition)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/lcm)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/include)

#-------------------------------------------------------------------------------
# Compiledefs for UUT
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_COMPILE_DEFS LCM_S)
list(APPEND UNIT_TEST_COMPILE_DEFS GPIO0_S)
list(APPEND UNIT_TEST_COMPILE_DEFS DEFAULT_VECTOR_TABLE_RESET=0x00000000)
list(APPEND UNIT_TEST_COMPILE_DEFS VM0_VECTOR_TABLE_RESET=0x10000000)
list(APPEND UNIT_TEST_COMPILE_DEFS QSPI_VECTOR_TABLE_RESET=0x20000000)
list(APPEND UNIT_TEST_COMPILE_DEFS SBAND_VECTOR_TABLE_RESET=0x30000000)

#-------------------------------------------------------------------------------
# Mocks for UUT
#-------------------------------------------------------------------------------
list(APPEND MOCK_HEADERS ${RSE_COMMON_SOURCE_DIR}/bringup_helpers/rse_bringup_helpers_hal.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/include/boot_hal.h)

#-------------------------------------------------------------------------------
# Labels for UT (Optional, tests can be grouped by labels)
#-------------------------------------------------------------------------------
list(APPEND UT_LABELS "BRINGUP_HELPERS")
