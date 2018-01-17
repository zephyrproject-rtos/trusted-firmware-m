#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file holds information of a specific build configuration of this project.

#Include board specific config (CPU, etc...)
include("Common/BoardSSE200")

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
set (REGRESSION False)
set (CORE_TEST True)
set (BL2 True)

include ("${CMAKE_CURRENT_LIST_DIR}/CommonConfig.cmake")

