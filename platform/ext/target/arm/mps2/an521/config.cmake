#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Set architecture and CPU
set(TFM_SYSTEM_PROCESSOR cortex-m33)
set(TFM_SYSTEM_ARCHITECTURE armv8-m.main)
set(TFM_SYSTEM_DSP OFF)
set(CONFIG_TFM_FP_ARCH "fpv5-d16")
set(CONFIG_TFM_FP_ARCH_ASM "FPv5_D16")

set(EXTRA_GENERATED_FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/arm/mps2/an521/generated_file_list.yaml)
list(FIND   TFM_EXTRA_GENERATED_FILE_LIST_PATH      ${EXTRA_GENERATED_FILE_LIST}    RET_VAL)
if(RET_VAL EQUAL -1)
    list(APPEND TFM_EXTRA_GENERATED_FILE_LIST_PATH ${EXTRA_GENERATED_FILE_LIST})
endif()

# Make FLIH IRQ test as the default IRQ test on AN521
set(TEST_NS_SLIH_IRQ                  OFF   CACHE BOOL    "Whether to build NS regression Second-Level Interrupt Handling tests")

if(BL2)
    set(BL2_TRAILER_SIZE 0x10000 CACHE STRING "Trailer size")
else()
    #No header if no bootloader, but keep IMAGE_CODE_SIZE the same
    set(BL2_TRAILER_SIZE 0x10400 CACHE STRING "Trailer size")
endif()

# Paramters for PSA API tests

set(PSA_API_TEST_TARGET              an521     CACHE STRING  "PSA_API_TARGET name")
set(PSA_API_TEST_CPU_ARCH            armv8m_ml CACHE STRING  "NSPE CPU_ARCH for PSA tests")

# Platform-specific configurations

set(CONFIG_TFM_USE_TRUSTZONE          ON)
set(TFM_MULTI_CORE_TOPOLOGY           OFF)

set(PLATFORM_HAS_ISOLATION_L3_SUPPORT ON)
set(PLATFORM_SLIH_IRQ_TEST_SUPPORT    ON)
set(PLATFORM_FLIH_IRQ_TEST_SUPPORT    ON)
