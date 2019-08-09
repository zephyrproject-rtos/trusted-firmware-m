#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Include BL2 bootloader related functions
include("${CMAKE_CURRENT_LIST_DIR}/MCUBoot.cmake")

set(BL2 True CACHE BOOL "Configure TF-M to use BL2 and enable building BL2")

if (BL2)
	add_definitions(-DBL2)

	set(MCUBOOT_IMAGE_NUMBER 1 CACHE STRING "Configure the number of separately updatable firmware images")
	set_property(CACHE MCUBOOT_IMAGE_NUMBER PROPERTY STRINGS "1")
	validate_cache_value(MCUBOOT_IMAGE_NUMBER STRINGS)

	set(MCUBOOT_UPGRADE_STRATEGY "OVERWRITE_ONLY" CACHE STRING "Configure BL2 which upgrade strategy to use")
	set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP;NO_SWAP;RAM_LOADING")
	validate_cache_value(MCUBOOT_UPGRADE_STRATEGY)

	set(MCUBOOT_SIGNATURE_TYPE "RSA-3072" CACHE STRING "Algorithm used by MCUBoot to validate signatures.")
	set_property(CACHE MCUBOOT_SIGNATURE_TYPE PROPERTY STRINGS "RSA-3072;RSA-2048")
	validate_cache_value(MCUBOOT_SIGNATURE_TYPE)

	if ((${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "NO_SWAP" OR
		 ${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "RAM_LOADING") AND
		NOT (MCUBOOT_IMAGE_NUMBER EQUAL 1))
		message(WARNING "The number of separately updatable images with the NO_SWAP or the RAM_LOADING"
			" upgrade strategy can be only '1'. Your choice was overriden.")
		set(MCUBOOT_IMAGE_NUMBER 1)
	endif()

else() #BL2 is turned off

	if (DEFINED MCUBOOT_IMAGE_NUMBER OR
		DEFINED MCUBOOT_UPGRADE_STRATEGY OR
		DEFINED MCUBOOT_SIGNATURE_TYPE)
		message(WARNING "Ignoring the values of MCUBOOT_* variables as BL2 option is set to False.")
		set(MCUBOOT_IMAGE_NUMBER "")
		set(MCUBOOT_UPGRADE_STRATEGY "")
		set(MCUBOOT_SIGNATURE_TYPE "")
	endif()

	if (DEFINED SECURITY_COUNTER)
		message(WARNING "Ignoring the value of SECURITY_COUNTER variable as BL2 option is set to False.")
		set(SECURITY_COUNTER "")
	endif()
endif()
