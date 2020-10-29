# Copyright (c) 2019, 2020 Linaro
# Copyright (c) 2020, Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0

# Adds trusted-firmware-m as an external project.
# Also creates a target called 'tfm_api'
# which can be linked into the app.
#
# When called from a Zephyr module, the following input values can be provided
# to configure the TF-M build:
#
# BINARY_DIR: The location where the build outputs will be written
# BOARD: The string identifying the board target for TF-M (AN521, etc.)
# CMAKE_BUILD_TYPE: The TF-M build type to use, (Debug, Release, etc.)
# IPC: Build TFM IPC library. This library allows a non-secure application to
#      interface to secure domain using IPC.
# ISOLATION_LEVEL: The TF-M isolation level to use
# REGRESSION: Boolean if TF-M build includes building the TF-M regression tests
# BL2: Boolean if the TF-M build uses MCUboot.
#
# Example usage:
#
# trusted_firmware_build(BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
#                        BOARD ${TFM_TARGET_PLATFORM}
#                        CMAKE_BUILD_TYPE Release
#                        IPC
#                        ISOLATION_LEVEL 2
#                        REGRESSION
#                        BL2 True
#                        BUILD_PROFILE profile_small)
function(trusted_firmware_build)
  set(options IPC REGRESSION)
  set(oneValueArgs BINARY_DIR BOARD BL2 ISOLATION_LEVEL CMAKE_BUILD_TYPE BUILD_PROFILE)
  cmake_parse_arguments(TFM "${options}" "${oneValueArgs}" "" ${ARGN})

  if(DEFINED TFM_BL2)
    set(TFM_BL2_ARG "-DBL2=${TFM_BL2}")
  endif()

  if(DEFINED TFM_IPC)
    set(TFM_IPC_ARG -DTFM_PSA_API=ON)
    # PSA API awareness for the Non-Secure application
    target_compile_definitions(app PRIVATE "TFM_PSA_API")
  endif()

  if(DEFINED TFM_ISOLATION_LEVEL)
    set(TFM_ISOLATION_LEVEL_ARG -DTFM_ISOLATION_LEVEL=${TFM_ISOLATION_LEVEL})
  endif()

  if(DEFINED TFM_REGRESSION)
    set(TFM_REGRESSION_ARG -DTEST_S=ON -DTEST_NS=ON)
  endif()

  if(DEFINED TFM_CMAKE_BUILD_TYPE)
    set(TFM_CMAKE_BUILD_TYPE_ARG -DCMAKE_BUILD_TYPE=${TFM_CMAKE_BUILD_TYPE})
  else()
    set(TFM_CMAKE_BUILD_TYPE_ARG -DCMAKE_BUILD_TYPE=RelWithDebInfo)
  endif()

  if(DEFINED TFM_BUILD_PROFILE)
    set(TFM_PROFILE_ARG -DTFM_PROFILE=${TFM_BUILD_PROFILE})
  endif()


  set(VENEERS_FILE ${TFM_BINARY_DIR}/secure_fw/s_veneers.o)
  set(PSA_API_NS_PATH ${TFM_BINARY_DIR}/interface/libpsa_api_ns.a)

  set(BUILD_BYPRODUCTS
    ${VENEERS_FILE}
    ${PSA_API_NS_PATH}
    ${BL2_HEX_FILE}
    )

  # Get the toolchain variant
  # TODO: Add support for cross-compile toolchain variant
  # TODO: Enforce GCC version check against TF-M compiler requirements
  if(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "zephyr")
    set(TFM_TOOLCHAIN_FILE "trusted-firmware-m/toolchain_GNUARM.cmake")
    set(TFM_TOOLCHAIN_PREFIX "arm-zephyr-eabi")
    set(TFM_TOOLCHAIN_PATH ${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi/bin)
  elseif(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "gnuarmemb")
    set(TFM_TOOLCHAIN_FILE "trusted-firmware-m/toolchain_GNUARM.cmake")
    set(TFM_TOOLCHAIN_PREFIX "arm-none-eabi")
    set(TFM_TOOLCHAIN_PATH ${GNUARMEMB_TOOLCHAIN_PATH}/bin)
  else()
    message(FATAL_ERROR "Unsupported ZEPHYR_TOOLCHAIN_VARIANT: ${ZEPHYR_TOOLCHAIN_VARIANT}")
  endif()

  include(ExternalProject)
  
  ExternalProject_Add(
    tfm
    SOURCE_DIR ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m
    BINARY_DIR ${TFM_BINARY_DIR}
    CMAKE_ARGS -DTFM_TOOLCHAIN_FILE=${ZEPHYR_TFM_MODULE_DIR}/${TFM_TOOLCHAIN_FILE}
               -DTFM_PLATFORM=${TFM_BOARD}
               -DCROSS_COMPILE=${TFM_TOOLCHAIN_PATH}/${TFM_TOOLCHAIN_PREFIX}
               ${TFM_CMAKE_BUILD_TYPE_ARG}
               ${TFM_BL2_ARG}
               ${TFM_IPC_ARG}
               ${TFM_ISOLATION_LEVEL_ARG}
               ${TFM_REGRESSION_ARG}
               ${TFM_PROFILE_ARG}
               -DTFM_TEST_REPO_PATH=${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests
               -DTFM_TOOLCHAIN_PATH=${TFM_TOOLCHAIN_PATH}
               -DTFM_TOOLCHAIN_PREFIX=${TFM_TOOLCHAIN_PREFIX}
    BUILD_ALWAYS True
    USES_TERMINAL_BUILD True
    BUILD_BYPRODUCTS ${BUILD_BYPRODUCTS}
  )

  add_library(tfm_api STATIC IMPORTED)
  set_target_properties(tfm_api PROPERTIES
    IMPORTED_LOCATION ${PSA_API_NS_PATH}
  )

  add_library(veneer_lib STATIC IMPORTED)
  set_target_properties(veneer_lib PROPERTIES
    IMPORTED_LOCATION ${VENEERS_FILE}
  )

  add_dependencies(tfm_api tfm)
  file(MAKE_DIRECTORY ${TFM_BINARY_DIR}/generated/interface/include)
  target_include_directories(tfm_api INTERFACE
    ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/include
    ${TFM_BINARY_DIR}/generated/interface/include
  )

  target_link_libraries(tfm_api INTERFACE zephyr_interface veneer_lib)

endfunction()
