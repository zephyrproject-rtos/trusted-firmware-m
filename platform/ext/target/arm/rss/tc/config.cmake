#-------------------------------------------------------------------------------
# Copyright (c) 2022-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Once all options are set, set common options as fallback
set(RSS_USE_SDS_LIB                     ON      CACHE BOOL    "Whether RSS should include SDS library or not")

include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
