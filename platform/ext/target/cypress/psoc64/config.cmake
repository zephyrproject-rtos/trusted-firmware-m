#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL2                                 OFF         CACHE BOOL      "Whether to build BL2")

set(TFM_PSA_API                         ON          CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode")

set(TFM_NS_CLIENT_IDENTIFICATION        OFF         CACHE BOOL      "Enable NS client identification")

############################ Platform ##########################################

set(TFM_MULTI_CORE_TOPOLOGY             ON          CACHE BOOL      "Whether to build for a dual-cpu architecture")
set(TFM_MULTI_CORE_MULTI_CLIENT_CALL    ON          CACHE BOOL      "Whether to enable multiple PSA client calls feature")

set(PLATFORM_DUMMY_ATTEST_HAL           FALSE       CACHE BOOL      "Use dummy attest hal implementation. Should not be used in production.")
set(PLATFORM_DUMMY_NV_COUNTERS          FALSE       CACHE BOOL      "Use dummy nv counter implementation. Should not be used in production.")
set(PLATFORM_DUMMY_CRYPTO_KEYS          FALSE       CACHE BOOL      "Use dummy crypto keys. Should not be used in production.")

# Disable default PDL IPC configuration. Use Cy_Platform_Init() in
# in psoc6_system_init_cm4.c and psoc6_system_init_cm0p.c instead.
add_definitions(-DCY_IPC_DEFAULT_CFG_DISABLE)
