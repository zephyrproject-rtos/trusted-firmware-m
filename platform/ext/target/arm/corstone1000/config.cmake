#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL1                                 ON         CACHE BOOL     "Whether to build BL1")
set(BL2                                 ON         CACHE BOOL     "Whether to build BL2")
set(BL2_TRAILER_SIZE                    0x800      CACHE STRING   "Trailer size")
set(DEFAULT_MCUBOOT_FLASH_MAP           OFF        CACHE BOOL     "Whether to use the default flash map defined by TF-M project")
set(MCUBOOT_UPGRADE_STRATEGY            "RAM_LOAD" CACHE STRING   "Upgrade strategy when multiple boot images are loaded [OVERWRITE_ONLY, SWAP, DIRECT_XIP, RAM_LOAD]")

set(TFM_ISOLATION_LEVEL                 2          CACHE STRING   "Isolation level")
set(MCUBOOT_IMAGE_NUMBER                2          CACHE STRING   "Whether to combine S and NS into either 1 image, or sign each separately")
set(TFM_MULTI_CORE_TOPOLOGY             ON         CACHE BOOL     "Whether to build for a dual-cpu architecture")
set(TFM_PLAT_SPECIFIC_MULTI_CORE_COMM   ON         CACHE BOOL     "Whether to use a platform specific inter core communication instead of mailbox in dual-cpu topology")
set(CRYPTO_HW_ACCELERATOR               ON         CACHE BOOL     "Whether to enable the crypto hardware accelerator on supported platforms")
set(CRYPTO_NV_SEED                      OFF        CACHE BOOL     "Use stored NV seed to provide entropy")
set(NS                                  FALSE      CACHE BOOL     "Whether to build NS app")

# External dependency on OpenAMP and Libmetal
set(LIBMETAL_SRC_PATH                   "DOWNLOAD"  CACHE PATH      "Path to Libmetal (or DOWNLOAD to fetch automatically")
set(LIBMETAL_VERSION                    "f252f0e007fbfb8b3a52b1d5901250ddac96baad"  CACHE STRING    "The version of libmetal to use")

set(LIBOPENAMP_SRC_PATH                 "DOWNLOAD"  CACHE PATH      "Path to Libopenamp (or DOWNLOAD to fetch automatically")
set(OPENAMP_VERSION                     "33037b04e0732e58fc0fa36afc244999ef632e10"  CACHE STRING    "The version of openamp to use")
