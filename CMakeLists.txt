# Copyright (c) 2019 Linaro
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Set default image versions if not defined elsewhere
if (NOT DEFINED TFM_IMAGE_VERSION_S)
	set(TFM_IMAGE_VERSION_S 0.0.0+0)
endif()

if (NOT DEFINED TFM_IMAGE_VERSION_NS)
	set(TFM_IMAGE_VERSION_NS 0.0.0+0)
endif()

function(trusted_firmware_build)
  set(oneValueArgs BINARY_DIR BOARD OUT_VENEERS_FILE OUT_INCLUDE_PATH)
  cmake_parse_arguments(TFM "" "${oneValueArgs}" "" ${ARGN})

  set(TFM_REMOTE_DIR "${ZEPHYR_TFM_MODULE_DIR}")
  set(TFM_BASE_DIR "${TFM_REMOTE_DIR}/trusted-firmware-m")

  set(VENEERS_FILE ${TFM_BINARY_DIR}/install/export/tfm/veneers/s_veneers.o)
  set(${TFM_OUT_VENEERS_FILE} ${VENEERS_FILE} PARENT_SCOPE)
  set(${TFM_OUT_INCLUDE_PATH} ${TFM_BINARY_DIR}/install/export/tfm/include PARENT_SCOPE)

  # Could be argument.
  set(TFMCONFIG "ConfigRegressionIPC")

  include(ExternalProject)

  ExternalProject_Add(
    tfm
    SOURCE_DIR ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m
    BINARY_DIR ${TFM_BINARY_DIR}
    BUILD_BYPRODUCTS ${VENEERS_FILE}
    CMAKE_ARGS -DPROJ_CONFIG=${TFM_BASE_DIR}/configs/${TFMCONFIG}.cmake
               -DTARGET_PLATFORM=${TFM_BOARD} -DBL2=True -DCOMPILER=GNUARM
               # These value should be updated when not using the Zephyr SDK
               -DGNUARM_PREFIX=arm-zephyr-eabi
               -DGNUARM_PATH=${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi
    BUILD_ALWAYS True
    USES_TERMINAL_BUILD True
  )
endfunction()
