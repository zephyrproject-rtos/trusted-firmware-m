#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file holds information of a specific build configuration of this project.

#Select platform specific build system settings file.
set(PLATFORM_CMAKE_FILE "${CMAKE_CURRENT_LIST_DIR}/platform/ext/Mps2AN521.cmake")

#Include board specific config (CPU, etc...)
include("Common/BoardAN521")

#Use any ARMCLANG version found on PATH. Note: Only versions supported by the
#build system will work. A file cmake/Common/CompilerArmClangXY.cmake
#must be present with a matching version.
include("Common/FindArmClang")
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/Common/${ARMCLANG_MODULE}.cmake")
    message(FATAL_ERROR "ERROR: Unsupported ARMCLANG compiler version found on PATH.")
endif()
include("Common/${ARMCLANG_MODULE}")

##These variables select how the projects are built. Each project will set
#various project specific settings (e.g. what files to build, macro
#definitions) based on these.
set (REGRESSION True)
set (CORE_TEST False)
set (BL2 True)

include ("${CMAKE_CURRENT_LIST_DIR}/CommonConfig.cmake")

