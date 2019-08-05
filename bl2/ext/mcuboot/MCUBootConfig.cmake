#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(BL2 True CACHE BOOL "Configure TF-M to use BL2 and enable building BL2")

if (BL2)
	add_definitions(-DBL2)

	set(MCUBOOT_UPGRADE_STRATEGY "OVERWRITE_ONLY" CACHE STRING "Configure BL2 which upgrade strategy to use")
	set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP;NO_SWAP;RAM_LOADING")
	validate_cache_value(MCUBOOT_UPGRADE_STRATEGY)

	set(MCUBOOT_SIGNATURE_TYPE "RSA-3072" CACHE STRING "Algorithm used by MCUBoot to validate signatures.")
	set_property(CACHE MCUBOOT_SIGNATURE_TYPE PROPERTY STRINGS "RSA-3072;RSA-2048")
	validate_cache_value(MCUBOOT_SIGNATURE_TYPE)

else() #BL2 is turned off
	if (DEFINED MCUBOOT_UPGRADE_STRATEGY OR
		DEFINED MCUBOOT_SIGNATURE_TYPE)
		message(WARNING "Ignoring the values of MCUBOOT_* variables as BL2 option is set to False.")
		set(MCUBOOT_UPGRADE_STRATEGY "")
		set(MCUBOOT_SIGNATURE_TYPE "")
	endif()

	if (DEFINED SECURITY_COUNTER)
		message(WARNING "Ignoring the value of SECURITY_COUNTER variable as BL2 option is set to False.")
		set(SECURITY_COUNTER "")
	endif()
endif()
