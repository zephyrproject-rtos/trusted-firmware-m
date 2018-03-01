#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file holds information of a specific build configuration of this project.

#Include board specific config (CPU, etc...), select platform specific build
#system settings file
if(NOT DEFINED TARGET_PLATFORM)
	message(FATAL_ERROR "ERROR: TARGET_PLATFORM is not set in command line")
elseif(${TARGET_PLATFORM} STREQUAL "AN521")
	set(PLATFORM_CMAKE_FILE "${CMAKE_CURRENT_LIST_DIR}/platform/ext/Mps2AN521.cmake")
elseif(${TARGET_PLATFORM} STREQUAL "AN519")
	set (PLATFORM_CMAKE_FILE "${CMAKE_CURRENT_LIST_DIR}/platform/ext/Mps2AN519.cmake")
elseif(${TARGET_PLATFORM} STREQUAL "MUSCA_A")
	set(PLATFORM_CMAKE_FILE "${CMAKE_CURRENT_LIST_DIR}/platform/ext/musca_a.cmake")
else()
	message(FATAL_ERROR "ERROR: Target \"${TARGET_PLATFORM}\" is not supported.")
endif()

##These variables select how the projects are built. Each project will set
#various project specific settings (e.g. what files to build, macro
#definitions) based on these.
set (REGRESSION True)
set (CORE_TEST False)

#BL2 bootloader(MCUBoot) related settings
set (BL2 True)
set (MCUBOOT_NO_SWAP False)

include ("${CMAKE_CURRENT_LIST_DIR}/CommonConfig.cmake")
