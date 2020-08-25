# Copyright (c) 2019, 2020 Linaro
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Adds trusted-firmware-m as an external project, and provides output values
# that are required to link against the secure TF-M binary.
#
# When called from a Zephyr module, the following input values can be provided
# to configure the TF-M build:
#
# BINARY_DIR: The location where the build outputs will be written
# BOARD: The string identifying the board target for TF-M (AN521, etc.)
# CFGFILE: The TF-M config file to use, without the .cmake extension
# IPC: Build TFM IPC library. This library allows a non-secure application to
#      interface to secure domain using IPC.
# BL2: Boolean if build uses MCUboot.
#
# The following output values can also be used:
#
# OUT_VENEERS_FILE: The path and filename of the veneer library to link against,
#                   which identifies where the veneer functions are in memory.
#                   Should be added via "target_link_libraries"
#
# Example usage:
#
# trusted_firmware_build(BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
#                        BOARD ${TFMBOARD}
#                        CFGFILE "ConfigRegressionIPC"
#                        OUT_VENEERS_FILE VENEERS_FILE
#                        OUT_INCLUDE_PATH TFM_INCLUDE_PATH
#                        BL2 True)
function(trusted_firmware_build)
  set(options IPC)
  set(oneValueArgs BINARY_DIR BOARD CFGFILE OUT_VENEERS_FILE OUT_INCLUDE_PATH BL2)
  cmake_parse_arguments(TFM "${options}" "${oneValueArgs}" "" ${ARGN})

  if(DEFINED TFM_BL2)
    set(TFM_BL2_ARG "-DBL2=${TFM_BL2}")
  endif()

  set(VENEERS_FILE ${TFM_BINARY_DIR}/install/export/tfm/veneers/s_veneers.o)
  set(${TFM_OUT_VENEERS_FILE} ${VENEERS_FILE} PARENT_SCOPE)
  set(${TFM_OUT_INCLUDE_PATH} ${TFM_BINARY_DIR}/install/export/tfm/include PARENT_SCOPE)

  # Get the toolchain variant
  # TODO: Add support for cross-compile toolchain variant
  # TODO: Enforce GCC version check against TF-M compiler requirements
  if(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "zephyr")
    set(TFM_TOOLCHAIN "GNUARM")
    set(TFM_TOOLCHAIN_PREFIX "arm-zephyr-eabi")
    set(TFM_TOOLCHAIN_PATH ${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi)
  elseif(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "gnuarmemb")
    set(TFM_TOOLCHAIN "GNUARM")
    set(TFM_TOOLCHAIN_PREFIX "arm-none-eabi")
    set(TFM_TOOLCHAIN_PATH ${GNUARMEMB_TOOLCHAIN_PATH})
  else()
    message(FATAL_ERROR "Unsupported ZEPHYR_TOOLCHAIN_VARIANT: ${ZEPHYR_TOOLCHAIN_VARIANT}")
  endif()

  include(ExternalProject)
  
  ExternalProject_Add(
    tfm
    SOURCE_DIR ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m
    BINARY_DIR ${TFM_BINARY_DIR}
    CMAKE_ARGS -DPROJ_CONFIG=${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/configs/${TFM_CFGFILE}.cmake
               -DTARGET_PLATFORM=${TFM_BOARD}
               ${TFM_BL2_ARG}
               -DCOMPILER=${TFM_TOOLCHAIN}
               -DGNUARM_PREFIX=${TFM_TOOLCHAIN_PREFIX}
               -DGNUARM_PATH=${TFM_TOOLCHAIN_PATH}
    BUILD_ALWAYS True
    USES_TERMINAL_BUILD True
    BUILD_BYPRODUCTS ${VENEERS_FILE}
  )

  set(TFM_INTERFACE_SOURCE ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/src)

  # IPC mode source dependencies
  if(TFM_IPC)
    add_library(tfm_ipc_psa_api
                ${TFM_INTERFACE_SOURCE}/tfm_psa_ns_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_crypto_ipc_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_initial_attestation_ipc_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_its_ipc_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_platform_ipc_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_ps_ipc_api.c
                ${TFM_INTERFACE_SOURCE}/tfm_ns_interface.c
    )
    target_include_directories(tfm_ipc_psa_api PUBLIC ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/include)
    target_compile_definitions(tfm_ipc_psa_api PUBLIC TFM_PSA_API TFM_PARTITION_TEST_CORE_IPC)
    target_link_libraries(tfm_ipc_psa_api PRIVATE zephyr_interface)
  endif()

endfunction()
