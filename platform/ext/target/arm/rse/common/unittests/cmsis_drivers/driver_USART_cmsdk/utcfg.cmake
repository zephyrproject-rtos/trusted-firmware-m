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
set(UNIT_UNDER_TEST ${RSE_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c)

#-------------------------------------------------------------------------------
# Test suite
#-------------------------------------------------------------------------------
set(UNIT_TEST_SUITE ${CMAKE_CURRENT_LIST_DIR}/test_driver_USART_cmsdk.c)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/unittests/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/partition)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/device/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/cmsis_drivers/config)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/include)

#-------------------------------------------------------------------------------
# Compiledefs for UUT
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_COMPILE_DEFS RSE_DEBUG_UART)
list(APPEND UNIT_TEST_COMPILE_DEFS UART0_CMSDK_NS)
list(APPEND UNIT_TEST_COMPILE_DEFS RTE_USART0=1)

#-------------------------------------------------------------------------------
# Link libs for UUT
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Mocks for UUT
#-------------------------------------------------------------------------------
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.h)

#-------------------------------------------------------------------------------
# Labels for UT (Optional, tests can be grouped by labels)
#-------------------------------------------------------------------------------
list(APPEND UT_LABELS "CMSIS_DRIVER")
