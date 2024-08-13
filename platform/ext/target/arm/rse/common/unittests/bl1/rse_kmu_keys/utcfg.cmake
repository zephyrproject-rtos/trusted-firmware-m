#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(PLATFORM_DIR ${TFM_ROOT_DIR}/platform)
set(RSE_COMMON_SOURCE_DIR ${PLATFORM_DIR}/ext/target/arm/rse/common)
set(CC3XX_SOURCE_DIR ${PLATFORM_DIR}/ext/target/arm/drivers/cc3xx)

#-------------------------------------------------------------------------------
# Unit under test
#-------------------------------------------------------------------------------
set(UNIT_UNDER_TEST ${RSE_COMMON_SOURCE_DIR}/bl1/rse_kmu_keys.c)

#-------------------------------------------------------------------------------
# Test suite
#-------------------------------------------------------------------------------

set(UNIT_TEST_SUITE ${CMAKE_CURRENT_LIST_DIR}/test_rse_kmu_keys.c)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Include dirs
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/unittests/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/partition)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/bl1/cc3xx)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR}/bl1)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${RSE_COMMON_SOURCE_DIR})
list(APPEND UNIT_TEST_INCLUDE_DIRS ${CC3XX_SOURCE_DIR}/common)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${CC3XX_SOURCE_DIR}/low_level_driver/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/lcm)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/ext/target/arm/drivers/kmu)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${PLATFORM_DIR}/include)
list(APPEND UNIT_TEST_INCLUDE_DIRS ${TFM_ROOT_DIR}/bl1/bl1_1/shared_lib/interface)

#-------------------------------------------------------------------------------
# Compiledefs for UUT
#-------------------------------------------------------------------------------
list(APPEND UNIT_TEST_COMPILE_DEFS PLATFORM_ERROR_CODES)
list(APPEND UNIT_TEST_COMPILE_DEFS RSE_BOOT_KEYS_CCA)
list(APPEND UNIT_TEST_COMPILE_DEFS RSE_BOOT_KEYS_DPE)
list(APPEND UNIT_TEST_COMPILE_DEFS RSE_XIP)
list(APPEND UNIT_TEST_COMPILE_DEFS KMU_S)
list(APPEND UNIT_TEST_COMPILE_DEFS LCM_S)

#-------------------------------------------------------------------------------
# Link libs for UUT
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Mocks for UUT
#-------------------------------------------------------------------------------
list(APPEND MOCK_HEADERS ${RSE_COMMON_SOURCE_DIR}/dpa_hardened_word_copy.h)
list(APPEND MOCK_HEADERS ${RSE_COMMON_SOURCE_DIR}/rse_boot_state.h)
list(APPEND MOCK_HEADERS ${CC3XX_SOURCE_DIR}/low_level_driver/include/cc3xx_kdf.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.h)
list(APPEND MOCK_HEADERS ${PLATFORM_DIR}/include/tfm_plat_otp.h)
list(APPEND MOCK_HEADERS ${TFM_ROOT_DIR}/bl1/bl1_1/shared_lib/interface/trng.h)

#-------------------------------------------------------------------------------
# Labels for UT (Optional, tests can be grouped by labels)
#-------------------------------------------------------------------------------
list(APPEND UT_LABELS "BL1")
