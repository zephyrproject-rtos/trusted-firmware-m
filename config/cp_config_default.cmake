#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

################################### FP ########################################

if (NOT DEFINED CONFIG_TFM_FP_ARCH)
    set(CONFIG_TFM_FP_ARCH "")
    return()
endif()

if (CONFIG_TFM_SPE_FP STREQUAL "1" OR CONFIG_TFM_SPE_FP STREQUAL "2")
    set(CONFIG_TFM_LAZY_STACKING_SPE     ON          CACHE BOOL      "Enable lazy stacking from SPE")
endif()
