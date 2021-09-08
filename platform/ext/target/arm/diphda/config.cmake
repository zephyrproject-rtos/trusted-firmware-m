#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL2                                 ON         CACHE BOOL     "Whether to build BL2")
set(DEFAULT_MCUBOOT_FLASH_MAP           OFF        CACHE BOOL     "Whether to use the default flash map defined by TF-M project")
set(DEFAULT_MCUBOOT_SECURITY_COUNTERS   OFF        CACHE BOOL     "Whether to use the default security counter configuration defined by TF-M project")
set(MCUBOOT_UPGRADE_STRATEGY            "RAM_LOAD" CACHE STRING   "Upgrade strategy when multiple boot images are loaded [OVERWRITE_ONLY, SWAP, DIRECT_XIP, RAM_LOAD]")

set(MCUBOOT_IMAGE_NUMBER                3          CACHE STRING   "Number of images loaded by mcuboot")
set(TFM_MULTI_CORE_TOPOLOGY             ON         CACHE BOOL     "Whether to build for a dual-cpu architecture")
set(TFM_PLAT_SPECIFIC_MULTI_CORE_COMM   ON         CACHE BOOL     "Whether to use a platform specific inter core communication instead of mailbox in dual-cpu topology")
set(CRYPTO_HW_ACCELERATOR               ON         CACHE BOOL     "Whether to enable the crypto hardware accelerator on supported platforms")
set(PLATFORM_DUMMY_NV_SEED              FALSE      CACHE BOOL      "Use dummy NV seed implementation. Should not be used in production.")
set(NS                                  FALSE      CACHE BOOL     "Whether to build NS app")

set(PLATFORM_DUMMY_ROTPK                FALSE      CACHE BOOL     "Use dummy root of trust public key. Dummy key is the public key for the default keys in bl2. Should not be used in production.")

# External dependency on OpenAMP and Libmetal
set(LIBMETAL_SRC_PATH                   "DOWNLOAD"  CACHE PATH      "Path to Libmetal (or DOWNLOAD to fetch automatically")
set(LIBMETAL_VERSION                    "f252f0e007fbfb8b3a52b1d5901250ddac96baad"  CACHE STRING    "The version of libmetal to use")

set(LIBOPENAMP_SRC_PATH                 "DOWNLOAD"  CACHE PATH      "Path to Libopenamp (or DOWNLOAD to fetch automatically")
set(OPENAMP_VERSION                     "33037b04e0732e58fc0fa36afc244999ef632e10"  CACHE STRING    "The version of openamp to use")
