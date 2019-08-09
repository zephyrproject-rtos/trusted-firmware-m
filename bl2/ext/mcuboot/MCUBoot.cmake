#-------------------------------------------------------------------------------
# Copyright (c) 2018-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

function(mcuboot_create_boot_payload)
	set( _OPTIONS_ARGS)                                          #Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS S_BIN NS_BIN FULL_BIN SIGN_BIN POSTFIX) #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS)                                      #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})

	if (NOT DEFINED _MY_PARAMS_S_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'S_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_NS_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'NS_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_FULL_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'FULL_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_SIGN_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'SIGN_BIN' missing.")
	endif()

	if (DEFINED _MY_PARAMS_POSTFIX)
		if (${_MY_PARAMS_POSTFIX} STREQUAL "_1")
			set(MY_POSTFIX "1")
		else()
			message(FATAL_ERROR "Unknown artefacts postfix: ${_MY_PARAMS_POSTFIX}")
		endif()
	endif()

	#Find Python3.x interpreter
	find_package(PythonInterp 3)
	if (NOT PYTHONINTERP_FOUND)
		message(FATAL_ERROR "Failed to find Python3.x interpreter. Pyhton3 must be installed and available on the PATH.")
	endif()

	if(NOT DEFINED FLASH_LAYOUT)
		message(FATAL_ERROR "ERROR: Incomplete Configuration: FLASH_LAYOUT is not defined.")
	endif()

	if (MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-3072")
		set(KEY_FILE "${MCUBOOT_DIR}/root-rsa-3072.pem")
	elseif(MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-2048")
		set(KEY_FILE "${MCUBOOT_DIR}/root-rsa-2048.pem")
	else()
		message(FATAL_ERROR "${MCUBOOT_SIGNATURE_TYPE} is not supported as firmware signing algorithm")
	endif()

	if (SECURITY_COUNTER)
		set (ADD_SECURITY_COUNTER "-s ${SECURITY_COUNTER}")
	else()
		set (ADD_SECURITY_COUNTER "")
	endif()

	set(FILE_TO_PREPROCESS ${CMAKE_BINARY_DIR}/image_macros_to_preprocess.c)
	set(PREPROCESSED_FILE ${CMAKE_BINARY_DIR}/image_macros_preprocessed.c)
	set(CONTENT_FOR_PREPROCESSING "#include \"${FLASH_LAYOUT}\"\n\n"
		"/* Enumeration that is used by the assemble.py and imgtool.py scripts\n"
		" * for correct binary generation when nested macros are used\n"
		" */\n"
		"enum image_attributes {\n"
		"\tRE_SECURE_IMAGE_OFFSET = SECURE_IMAGE_OFFSET,\n"
		"\tRE_SECURE_IMAGE_MAX_SIZE = SECURE_IMAGE_MAX_SIZE,\n"
		"\tRE_NON_SECURE_IMAGE_OFFSET = NON_SECURE_IMAGE_OFFSET,\n"
		"\tRE_NON_SECURE_IMAGE_MAX_SIZE = NON_SECURE_IMAGE_MAX_SIZE,\n"
		"#ifdef IMAGE_LOAD_ADDRESS\n"
		"\tRE_IMAGE_LOAD_ADDRESS = IMAGE_LOAD_ADDRESS,\n"
		"#endif\n"
		"\tRE_SIGN_BIN_SIZE = SIGN_BIN_SIZE\n}\;"
	)

	#Create a file that will be preprocessed later in order to be able to handle nested macros
	#in header files for certain macros
	file(WRITE ${FILE_TO_PREPROCESS} ${CONTENT_FOR_PREPROCESSING})

	#Preprocess the .c file that contains the image related macros
	compiler_preprocess_file(SRC ${FILE_TO_PREPROCESS}
							DST ${PREPROCESSED_FILE}
							BEFORE_TARGET ${_MY_PARAMS_NS_BIN}
							TARGET_PREFIX ${_MY_PARAMS_NS_BIN}
							DEFINES "MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}")

	add_custom_command(TARGET ${_MY_PARAMS_NS_BIN}
						POST_BUILD
						#Create concatenated binary image from the two binary file
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/assemble.py
						ARGS --layout ${PREPROCESSED_FILE}
							 -s $<TARGET_FILE_DIR:${_MY_PARAMS_S_BIN}>/${_MY_PARAMS_S_BIN}.bin
							 -n $<TARGET_FILE_DIR:${_MY_PARAMS_NS_BIN}>/${_MY_PARAMS_NS_BIN}.bin
							 -o ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin

						#Sign concatenated binary image with default public key in mcuboot folder
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/imgtool.py
						ARGS sign
							 --layout ${PREPROCESSED_FILE}
							 -k ${KEY_FILE}
							 --align 1
							 -v ${IMAGE_VERSION}
							 ${ADD_SECURITY_COUNTER}
							 -H 0x400
							 ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin
							 ${CMAKE_BINARY_DIR}/${_MY_PARAMS_SIGN_BIN}.bin)

	#Collect executables to common location: build/install/outputs/
	set(TFM_FULL_NAME tfm_s_ns_concatenated)
	set(TFM_SIGN_NAME tfm_s_ns_signed)

	if (DEFINED MY_POSTFIX)
		install(FILES  ${CMAKE_BINARY_DIR}/${_MY_PARAMS_SIGN_BIN}.bin
				RENAME tfm_sig${MY_POSTFIX}.bin
				DESTINATION outputs/${TARGET_PLATFORM}/)
	else()
		install(FILES ${CMAKE_BINARY_DIR}/${_MY_PARAMS_SIGN_BIN}.bin
				DESTINATION outputs/${TARGET_PLATFORM}/)
	endif()

	install(FILES  ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin
			DESTINATION outputs/${TARGET_PLATFORM}/)

	install(FILES  ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin
			RENAME ${TFM_FULL_NAME}${_MY_PARAMS_POSTFIX}.bin
			DESTINATION outputs/fvp/)

	install(FILES  ${CMAKE_BINARY_DIR}/${_MY_PARAMS_SIGN_BIN}.bin
			RENAME ${TFM_SIGN_NAME}${_MY_PARAMS_POSTFIX}.bin
			DESTINATION outputs/fvp/)
endfunction()

#Validate and override the upgrade strategy to be used by the bootloader.
#
# If the given upgrade strategy is not supported with the current value
# of the MCUBOOT_IMAGE_NUMBER variable then the function will override its
# previously set value.
#
#Examples:
#  mcuboot_override_upgrade_strategy("SWAP")
#
#INPUTS:
#  strategy - (mandatory) - Upgrade strategy to be used.
#
#OUTPUTS:
#  MCUBOOT_UPGRADE_STRATEGY variable is set to the new strategy.
#
function(mcuboot_override_upgrade_strategy strategy)
	if ((${strategy} STREQUAL "NO_SWAP" OR
		 ${strategy} STREQUAL "RAM_LOADING") AND
		NOT (MCUBOOT_IMAGE_NUMBER EQUAL 1))
		message(WARNING "The number of separately updatable images with the NO_SWAP or the RAM_LOADING"
			" upgrade strategy can be only '1'. Your choice was overriden.")
		set(MCUBOOT_IMAGE_NUMBER 1 PARENT_SCOPE)
	endif()
	get_property(_validation_list CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS)
	#Check if validation list is set.
	if (NOT _validation_list)
		#Set the default upgrade strategy if the CACHE variable has not been set yet.
		set(MCUBOOT_UPGRADE_STRATEGY "OVERWRITE_ONLY" CACHE STRING "Configure BL2 which upgrade strategy to use")
		set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP;NO_SWAP;RAM_LOADING")
	endif()
	set(MCUBOOT_UPGRADE_STRATEGY ${strategy} PARENT_SCOPE)
	validate_cache_value(MCUBOOT_UPGRADE_STRATEGY STRINGS)
endfunction()
