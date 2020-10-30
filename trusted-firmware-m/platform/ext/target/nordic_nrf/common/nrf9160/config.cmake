#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(SECURE_UART1                        ON         CACHE BOOL      "Enable secure UART1" FORCE)
set(PSA_API_TEST_TARGET                 "nrf"      CACHE STRING    "PSA API test target" FORCE)
set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/${NRF_PATH}/common/core/generated_file_list.yaml          CACHE PATH      "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)

set(FLASH_AREA_START                    0x0        CACHE STRING      "Start of the flash area reserved for TF-M.")
set(FLASH_AREA_BL2_SIZE                 0x10000    CACHE STRING      "Partition size for BL2.")
set(FLASH_S_PARTITION_SIZE              0x40000    CACHE STRING      "Partition size for secure flash.")
set(FLASH_NS_PARTITION_SIZE             0x30000    CACHE STRING      "Partition size for nonsecure flash.")
set(FLASH_AREA_END                      0x100000   CACHE STRING      "The end of the flash reserved for TF-M.")

set(SRAM_AREA_START                     0x0        CACHE STRING      "Start of the RAM area reserved for TF-M.")
# Assign to SPE the minimum amount of RAM (aligned to the SPU region boundary)
# that is needed for the most demanding configuration, which turns out to be
# the RegressionIPC one.
set(SRAM_S_PARTITION_SIZE               0x16000    CACHE STRING      "Partition size for secure RAM.")
set(SRAM_NS_PARTITION_SIZE              0x2a000    CACHE STRING      "Partition size for nonsecure RAM.")

add_compile_definitions(
    FLASH_AREA_START=${FLASH_AREA_START}
    FLASH_AREA_BL2_SIZE=${FLASH_AREA_BL2_SIZE}
    FLASH_S_PARTITION_SIZE=${FLASH_S_PARTITION_SIZE}
    FLASH_NS_PARTITION_SIZE=${FLASH_NS_PARTITION_SIZE}
    FLASH_AREA_END=${FLASH_AREA_END}
    SRAM_AREA_START=${SRAM_AREA_START}
    SRAM_S_PARTITION_SIZE=${SRAM_S_PARTITION_SIZE}
    SRAM_NS_PARTITION_SIZE=${SRAM_NS_PARTITION_SIZE}
)
