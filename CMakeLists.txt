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

  # Determine TF-M platform target
  if(CONFIG_TFM_BOARD_LPCXPRESSO55S69)
    set(TFM_PLATFORM_DIR "nxp/lpcxpresso55s69")
  elseif(CONFIG_TFM_BOARD_MPS2_AN521)
    set(TFM_PLATFORM_DIR "mps2/an521")
  elseif(CONFIG_TFM_BOARD_MUSCA_B1)
    set(TFM_PLATFORM_DIR "musca_b1")
  elseif(CONFIG_TFM_BOARD_NRF5340DK_NRF5340)
    set(TFM_PLATFORM_DIR "nordic_nrf/nrf5340dk_nrf5340_cpuapp")
  elseif(CONFIG_TFM_BOARD_NRF5340PDK_NRF5340)
    set(TFM_PLATFORM_DIR "nordic_nrf/nrf5340pdk_nrf5340_cpuapp")
  elseif(CONFIG_TFM_BOARD_NRF9160DK_NRF9160)
    set(TFM_PLATFORM_DIR "nordic_nrf/nrf9160dk_nrf9160")
  elseif(CONFIG_TFM_BOARD_BOARD_NUCLEO_L552ZE_Q)
    set(TFM_PLATFORM_DIR "stm/nucleo_l552ze_q")
  else()
    message(FATAL_ERROR "Unsupported TFM BOARD")
  endif()

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
  set(TFM_GENERATED_INCLUDES ${TFM_BINARY_DIR}/generated/interface/include)

  set(BL2_HEX_FILE ${CMAKE_BINARY_DIR}/tfm/bin/bl2.hex)

  set(BUILD_BYPRODUCTS
    ${VENEERS_FILE}
    ${PSA_API_NS_PATH}
    ${BL2_HEX_FILE}
    ${TFM_GENERATED_INCLUDES}/psa_manifest/sid.h
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
               -DTFM_PLATFORM=${TFM_PLATFORM_DIR}
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

  add_library(tfm_api
    ${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests/app/os_wrapper_cmsis_rtos_v2.c
  )

  target_include_directories(tfm_api
    PRIVATE
    ${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests/CMSIS/RTOS2/Include
    PUBLIC
    ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/include
    INTERFACE
    ${TFM_GENERATED_INCLUDES}
  )

  target_link_libraries(tfm_api
    PUBLIC
    zephyr_interface
    INTERFACE
    ${PSA_API_NS_PATH}
    ${VENEERS_FILE}
    $<TARGET_FILE:tfm_api>
  )

  add_dependencies(tfm_api tfm)
endfunction()

if (CONFIG_BUILD_WITH_TFM)
  if (CONFIG_TFM_IPC)
    set(TFM_IPC_ARG IPC)
  endif()
  if (CONFIG_TFM_REGRESSION)
    set(TFM_REGRESSION_ARG REGRESSION)
  endif()
  if (CONFIG_TFM_BL2_TRUE)
    set(TFM_BL2_ARG BL2 True)
  elseif (CONFIG_TFM_BL2_FALSE)
    set(TFM_BL2_ARG BL2 False)
  endif()
  if (CONFIG_TFM_ISOLATION_LEVEL)
    set(TFM_ISOLATION_LEVEL_ARG ISOLATION_LEVEL ${CONFIG_TFM_ISOLATION_LEVEL})
  endif()
  if (CONFIG_TFM_PROFILE)
    set(TFM_PROFILE_ARG BUILD_PROFILE ${CONFIG_TFM_PROFILE})
  endif()

  trusted_firmware_build(
    BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
    BOARD ${CONFIG_TFM_BOARD}
    ${TFM_ISOLATION_LEVEL_ARG}
    ${TFM_PROFILE_ARG}
    ${TFM_BL2_ARG}
    ${TFM_IPC_ARG}
    ${TFM_REGRESSION_ARG}
  )

  zephyr_link_libraries(tfm_api)
endif()
