#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL2                                 OFF         CACHE BOOL      "Whether to build BL2" FORCE)

set(TFM_PSA_API                         ON          CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode" FORCE)

set(TFM_NS_CLIENT_IDENTIFICATION        OFF         CACHE BOOL      "Enable NS client identification" FORCE)

############################ Platform ##########################################

set(TFM_MULTI_CORE_TOPOLOGY             ON          CACHE BOOL      "Whether to build for a dual-cpu architecture" FORCE)

set(PLATFORM_DUMMY_ATTEST_HAL           FALSE       CACHE BOOL      "Use dummy attest hal implementation. Should not be used in production." FORCE)
set(PLATFORM_DUMMY_NV_COUNTERS          FALSE       CACHE BOOL      "Use dummy nv counter implementation. Should not be used in production." FORCE)
set(PLATFORM_DUMMY_CRYPTO_KEYS          FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production." FORCE)
