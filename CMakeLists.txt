# Copyright (c) 2019, 2020 Linaro
# Copyright (c) 2020, Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

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
# BL2: Boolean if build uses MCUboot.
# NATIVE_NS: Use the TF-M-built NS app instead of the Zephyr app.
#
# Example usage:
#
# trusted_firmware_build(BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
#                        BOARD ${TFM_TARGET_PLATFORM}
#                        CMAKE_BUILD_TYPE Release
#                        IPC
#                        ISOLATION_LEVEL 2
#                        REGRESSION
#                        BL2 True)
function(trusted_firmware_build)
  set(options IPC REGRESSION NATIVE_NS)
  set(oneValueArgs BINARY_DIR BOARD BL2 ISOLATION_LEVEL CMAKE_BUILD_TYPE)
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

  set(VENEERS_FILE ${TFM_BINARY_DIR}/secure_fw/s_veneers.o)
  set(PSA_API_NS_PATH ${TFM_BINARY_DIR}/interface/libpsa_api_ns.a)
  set(PLATFORM_NS_PATH ${TFM_BINARY_DIR}/platform/libplatform_ns.a)

  set(BL2_HEX_FILE ${CMAKE_BINARY_DIR}/tfm/bin/bl2.hex)
  set_property(GLOBAL PROPERTY
    bl2_PM_HEX_FILE
    ${BL2_HEX_FILE}
    )

  set(BUILD_BYPRODUCTS
    ${VENEERS_FILE}
    ${PSA_API_NS_PATH}
    ${PLATFORM_NS_PATH}
    ${BL2_HEX_FILE}
    ${NATIVE_NS_BIN_FILE}
    )

  # Get the toolchain variant
  # TODO: Add support for cross-compile toolchain variant
  # TODO: Enforce GCC version check against TF-M compiler requirements
  if(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "zephyr")
    set(TFM_TOOLCHAIN "GNUARM")
    set(CMAKE_TOOLCHAIN_FILE "trusted-firmware-m/toolchain_GNUARM.cmake")
    set(TFM_TOOLCHAIN_PREFIX "arm-zephyr-eabi")
    set(TFM_TOOLCHAIN_PATH ${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi)
  elseif(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "gnuarmemb")
    set(TFM_TOOLCHAIN "GNUARM")
    set(CMAKE_TOOLCHAIN_FILE "trusted-firmware-m/toolchain_GNUARM.cmake")
    set(TFM_TOOLCHAIN_PREFIX "arm-none-eabi")
    set(TFM_TOOLCHAIN_PATH ${GNUARMEMB_TOOLCHAIN_PATH})
  else()
    message(FATAL_ERROR "Unsupported ZEPHYR_TOOLCHAIN_VARIANT: ${ZEPHYR_TOOLCHAIN_VARIANT}")
  endif()

  set(flash_end 0x100000)

  include(ExternalProject)

  ExternalProject_Add(
    tfm
    SOURCE_DIR ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m
    BINARY_DIR ${TFM_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${ZEPHYR_TFM_MODULE_DIR}/${CMAKE_TOOLCHAIN_FILE}
               -DTFM_PLATFORM=${TFM_BOARD}
               ${TFM_CMAKE_BUILD_TYPE_ARG}
               ${TFM_BL2_ARG}
               ${TFM_IPC_ARG}
               ${TFM_ISOLATION_LEVEL_ARG}
               ${TFM_REGRESSION_ARG}
               -DTFM_TEST_REPO_PATH=${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests
               -DTFM_TOOLCHAIN_PATH=${TFM_TOOLCHAIN_PATH}
               -DTFM_TOOLCHAIN_PREFIX=${TFM_TOOLCHAIN_PREFIX}

               -DFLASH_AREA_START=$<TARGET_PROPERTY:partition_manager,PM_BL2_ADDRESS>
               -DFLASH_AREA_BL2_SIZE=$<TARGET_PROPERTY:partition_manager,PM_BL2_SIZE>
               -DFLASH_S_PARTITION_SIZE=$<TARGET_PROPERTY:partition_manager,PM_TFM_SIZE>
               -DFLASH_NS_PARTITION_SIZE=$<TARGET_PROPERTY:partition_manager,PM_APP_SIZE>
               -DFLASH_AREA_END=$<TARGET_PROPERTY:partition_manager,PM_TFM_EXTRA_END_ADDRESS>
               -DSRAM_AREA_START=$<TARGET_PROPERTY:partition_manager,PM_TFM_RAM_ADDRESS>
               -DSRAM_S_PARTITION_SIZE=$<TARGET_PROPERTY:partition_manager,PM_TFM_RAM_SIZE>
               -DSRAM_NS_PARTITION_SIZE=$<TARGET_PROPERTY:partition_manager,PM_SRAM_PRIMARY_SIZE>
    BUILD_ALWAYS True
    USES_TERMINAL_BUILD True
    BUILD_BYPRODUCTS ${BUILD_BYPRODUCTS}
  )

  if (TFM_NATIVE_NS)
    set(NATIVE_NS_BIN_FILE ${CMAKE_BINARY_DIR}/tfm/bin/tfm_s_ns_signed.bin)
    set(NATIVE_NS_HEX_FILE ${CMAKE_BINARY_DIR}/tfm/bin/tfm_s_ns_signed.hex)

    # Set srec_cat binary name
    find_program(SREC_CAT srec_cat)
    if(${SREC_CAT} STREQUAL SREC_CAT-NOTFOUND)
        message(FATAL_ERROR "'srec_cat' not found. Please install it, or add it to $PATH.")
    endif()

    # Generate an intel hex file from the signed output binary
    add_custom_command(
      OUTPUT ${NATIVE_NS_HEX_FILE}
      COMMAND srec_cat ${NATIVE_NS_BIN_FILE}
        -binary
        -offset 0x10000
        -o ${NATIVE_NS_HEX_FILE}
        -intel
      )
    add_custom_target(tfm_native_ns DEPENDS ${NATIVE_NS_HEX_FILE} tfm)

    set_property(GLOBAL PROPERTY
      signed_tfm_app_PM_HEX_FILE
      ${NATIVE_NS_HEX_FILE}
      )
    set_property(GLOBAL PROPERTY
      signed_tfm_app_PM_TARGET
      tfm_native_ns
      )
  endif()

  add_library(tfm_api
    ${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests/app/os_wrapper_cmsis_rtos_v2.c
    ${ZEPHYR_TFM_MODULE_DIR}/src/zephyr_tfm_log.c
  )

  if (TFM_PSA_TEST_SUITE)
    add_library(psa_test_lib STATIC IMPORTED)
    set_target_properties(psa_test_lib PROPERTIES
      IMPORTED_LOCATION ${PSA_TESTS_NS_PATH}
    )
    target_link_libraries(psa_test_lib INTERFACE tfm_api)
  endif()

  add_library(interface_lib STATIC IMPORTED)
  set_target_properties(interface_lib PROPERTIES
    IMPORTED_LOCATION ${PSA_API_NS_PATH}
  )
  target_link_libraries(interface_lib INTERFACE tfm_api)

  add_library(platform_lib STATIC IMPORTED)
  set_target_properties(platform_lib PROPERTIES
    IMPORTED_LOCATION ${PLATFORM_NS_PATH}
  )
  target_link_libraries(platform_lib INTERFACE tfm_api)

  add_library(veneer_lib STATIC IMPORTED)
  set_target_properties(veneer_lib PROPERTIES
    IMPORTED_LOCATION ${VENEERS_FILE}
  )

  add_dependencies(interface_lib tfm)
  file(MAKE_DIRECTORY ${TFM_BINARY_DIR}/generated/interface/include)

  target_include_directories(tfm_api
    PRIVATE
    ${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests/CMSIS/RTOS2/Include
    PUBLIC
    ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/include
    INTERFACE
    ${TFM_BINARY_DIR}/generated/interface/include
  )
  add_dependencies(tfm_api tfm)

  target_link_libraries(tfm_api PUBLIC
    zephyr_interface
    interface_lib
    platform_lib
    veneer_lib
  )

endfunction()

if (CONFIG_TFM)
  if (CONFIG_TFM_IPC)
    set(TFM_IPC_ARG IPC)
  endif()
  if (CONFIG_TFM_REGRESSION)
    set(TFM_REGRESSION_ARG REGRESSION)
  endif()
  if (CONFIG_TFM_BL2)
    set(TFM_BL2_ARG BL2 True)
  else()
    set(TFM_BL2_ARG BL2 False)
  endif()
  if (CONFIG_TFM_NATIVE_NS)
    set(TFM_NATIVE_NS_ARG NATIVE_NS)
  endif()

  message("CONFIG_TFM_BOARD: ${CONFIG_TFM_BOARD}")
  trusted_firmware_build(
    BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
    BOARD ${CONFIG_TFM_BOARD}
    ISOLATION_LEVEL ${CONFIG_TFM_ISOLATION_LEVEL}
    ${TFM_BL2_ARG}
    ${TFM_IPC_ARG}
    ${TFM_REGRESSION_ARG}
    ${TFM_NATIVE_NS_ARG}
  )

  zephyr_link_libraries(tfm_api)
endif()
