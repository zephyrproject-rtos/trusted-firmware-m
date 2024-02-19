#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Once all options are set, set common options as fallback
if (TFM_PARTITION_DPE)
    set(RSE_USE_SDS_LIB         ON  CACHE BOOL  "Whether RSE should include SDS library or not")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
