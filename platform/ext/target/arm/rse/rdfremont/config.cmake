#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Note: Cached varaibles always take the first value set, while normal
# variables always take the last.

set(TFM_BL1_MEMORY_MAPPED_FLASH         OFF      CACHE BOOL    "Whether BL1 can directly access flash content")
set(TFM_ATTESTATION_SCHEME              "CCA"    CACHE STRING  "Attestation scheme to use [OFF, PSA, CCA, DPE]")

set(PLATFORM_HAS_BOOT_DMA               OFF      CACHE BOOL    "Enable dma support for memory transactions for bootloader")
set(PLATFORM_HAS_NI_TOWER               ON       CACHE BOOL    "Enable NI-Tower support")
set(PLATFORM_HAS_SMMU_V3                ON       CACHE BOOL    "Enable SMMU V3 support")

# This sub-platform uses a mix of MHUv2 in BL1 and MHUv3 in other firmware.
# Because of this, PLAT_MHU_VERSION cannot be set to 2 or 3, which would add
# either MHUv2 or MHUv3 for all firmware. To work around this, set
# PLAT_MHU_VERSION to 0 so neither gets added by the common cmake and
# sub-platform specific cmake can be used to handle the mix of MHUv2 and MHUv3.
set(PLAT_MHU_VERSION                    0        CACHE STRING  "Supported MHU version by platform")
set(MCUBOOT_IMAGE_NUMBER                6        CACHE STRING  "Number of images supported by MCUBoot")
set(RSE_USE_HOST_UART                   OFF      CACHE BOOL    "Whether RSE should use the UART from the host system (opposed to dedicated UART private to RSE)")
set(RSE_USE_HOST_FLASH                  OFF      CACHE BOOL    "Enable RSE using the host flash.")
set(RSE_HAS_EXPANSION_PERIPHERALS       ON       CACHE BOOL    "Whether RSE has sub-platform specific peripherals in the expansion layer")
if (TFM_PLATFORM_VARIANT STREQUAL "1")
    set(PLAT_LCP_COUNT                      8        CACHE STRING  "Number of LCPs to load")
elseif (TFM_PLATFORM_VARIANT STREQUAL "2")
    set(PLAT_LCP_COUNT                      4        CACHE STRING  "Number of LCPs to load")
    set(RSE_AMOUNT                          4        CACHE STRING  "Amount of RSEs in the system")
    set(MULTI_RSE_TOPOLOGY_FILE             ${CMAKE_CURRENT_LIST_DIR}/bl1/rdfremontcfg2.tgf CACHE STRING "Topology file for RSE to RSE BL1 communication")
else()
    set(PLAT_LCP_COUNT                      16       CACHE STRING  "Number of LCPs to load")
endif()
set(RSE_LOAD_NS_IMAGE                   OFF      CACHE BOOL    "Whether to load an NS image")
set(CONFIG_TFM_SPM_BACKEND              IPC      CACHE STRING  "The SPM backend")

# Once all cache options are set, set common options as fallback
include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
