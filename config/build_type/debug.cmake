#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(MBEDCRYPTO_BUILD_TYPE               relwithdebinfo CACHE STRING "Build type of Mbed Crypto library")
set(TFM_SPM_LOG_LEVEL                   3              CACHE STRING "Set debug SPM log level as Debug level" FORCE)
set(TFM_PARTITION_LOG_LEVEL             TFM_PARTITION_LOG_LEVEL_DEBUG CACHE STRING "Set debug SP log level as Debug level")
