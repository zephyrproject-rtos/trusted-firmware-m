#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
# Copyright (c) 2021, Nordic Semiconductor ASA.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(HAL_NORDIC_PATH "DOWNLOAD" CACHE PATH "Path to the Nordic HAL (or DOWNLOAD to fetch automatically)")
set(HAL_NORDIC_VERSION "004b62c0b74c62bd8489e0a702a498899e0463d1" CACHE STRING "Version of the Nordic HAL to download") # nrfx 2.6.0
set(HAL_NORDIC_REMOTE "https://github.com/zephyrproject-rtos/hal_nordic" CACHE STRING "Remote of the Nordic HAL to download")

set(CONFIG_TFM_USE_TRUSTZONE            ON          CACHE BOOL    "Enable use of TrustZone to transition between NSPE and SPE")
set(TFM_MULTI_CORE_TOPOLOGY             OFF         CACHE BOOL    "Whether to build for a dual-cpu architecture")

set(NULL_POINTER_EXCEPTION_DETECTION    FALSE       CACHE BOOL
  "Enable null-pointer dereference detection for \
   priviliged and unpriviliged secure reads and writes on supported platforms. \
   Can be used to debug faults in the SPE. \
   Note that null-pointer dereferences from the NSPE \
   will trigger SecureFaults even without this option enabled. \
   May require more MPU regions than are available depending on the configuration."
  )

set(PLATFORM_SLIH_IRQ_TEST_SUPPORT      ON          CACHE BOOL    "Platform supports SLIH IRQ tests")
set(PLATFORM_FLIH_IRQ_TEST_SUPPORT      ON          CACHE BOOL    "Platform supports FLIH IRQ tests")

# Required if MCUBoot has been built without CONFIG_MCUBOOT_CLEANUP_ARM_CORE enabled
set(NRF_HW_INIT_RESET_ON_BOOT OFF CACHE BOOL "Initialize internal architecture state at boot")

# Required if MCUboot has been built without CONFIG_MCUBOOT_NRF_CLEANUP_PERIPHERAL enabled.
set(NRF_HW_INIT_NRF_PERIPHERALS OFF CACHE BOOL "Initialize nRF peripherals at boot")

if (NRF_HW_INIT_NRF_PERIPHERALS AND NOT NRF_HW_INIT_RESET_ON_BOOT)
        message(FATAL_ERROR "NRF_HW_INIT_NRF_PERIPHERALS depends on NRF_HW_INIT_RESET_ON_BOOT")
endif()
