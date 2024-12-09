#-------------------------------------------------------------------------------
# Portions Copyright (C) 2024-2025 Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL2                                 ON         CACHE BOOL     "Whether to build BL2")
set(CONFIG_TFM_USE_TRUSTZONE            ON)
set(TFM_PARTITION_PLATFORM              OFF          CACHE BOOL      "Enable Platform partition")
set(TFM_PARTITION_CRYPTO                ON          CACHE BOOL      "Enable Crypto partition")
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON       CACHE BOOL      "Enable Internal Trusted Storage partition")
set(TFM_PARTITION_NS_AGENT_TZ           ON         CACHE BOOL      "Enable Non-Secure Agent in Secure partition")
set(CONFIG_TFM_BOOT_STORE_MEASUREMENTS          OFF  CACHE BOOL      "Store measurement values from all the boot stages. Used for initial attestation token.")
set(CONFIG_TFM_BOOT_STORE_ENCODED_MEASUREMENTS  OFF  CACHE BOOL      "Enable storing of encoded measurements in boot.")
set(CONFIG_TFM_HALT_ON_CORE_PANIC               ON   CACHE BOOL      "On fatal errors in the secure firmware, halt instead of rebooting.")
set(PLATFORM_DEFAULT_OTP                OFF          CACHE BOOL      "Use trusted on-chip flash to implement OTP memory")

set(HAL_ADI_PATH                        "DOWNLOAD"  CACHE PATH      "Path to hal_adi (or DOWNLOAD to fetch automatically")
set(HAL_ADI_VERSION                     "dd1c525"   CACHE STRING    "The version of hal_adi to use")
