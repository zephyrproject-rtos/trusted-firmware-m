#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all musca specific files in the application.

#MUSCA-A has a Cortex M33 CPU.
include("Common/CpuM33")

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

#Specify the location of platform specific build dependencies.
if(COMPILER STREQUAL "ARMCLANG")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/musca_bl2.sct")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/musca_s.sct")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/musca_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/musca_bl2.ld")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/musca_s.ld")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/musca_ns.ld")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT          "${PLATFORM_DIR}/target/musca_a/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${PLATFORM_DIR}/target/musca_a/partition")
set (SIGN_BIN_SIZE         0x30000)

if (BL2)
  set (BL2_LINKER_CONFIG ${BL2_SCATTER_FILE_NAME})
endif()

embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver/Config" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/Device/Config" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/Device/Include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/Native_Driver" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/partition" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/Device/Source/system_cmsdk_musca.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/Device/Source/platform_retarget_dev.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  if (NOT DEFINED SECURE_UART1)
    message(FATAL_ERROR "Configuration variable SECURE_UART1 (true|false) is undefined!")
  elseif(SECURE_UART1)
    message(FATAL_ERROR "Configuration SECURE_UART1 TRUE is invalid for this target!")
  endif()
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/Native_Driver/uart_pl011_drv.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/Native_Driver/musca_a1_scc_drv.c")

  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/musca_a/Native_Driver/mpc_sie200_drv.c"
              "${PLATFORM_DIR}/target/musca_a/Native_Driver/ppc_sse200_drv.c")
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/Native_Driver/timer_cmsdk.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/Native_Driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/startup_cmsdk_musca_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/startup_cmsdk_musca_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/musca_a/Device/Source/armclang/startup_cmsdk_musca_bl2.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/startup_cmsdk_musca_s.S")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/startup_cmsdk_musca_ns.S")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/musca_a/Device/Source/gcc/startup_cmsdk_musca_bl2.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
      PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/musca_a/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/musca_a/Native_Driver/mpu_armv8m_drv.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_TARGET_HARDWARE_KEYS)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_HARDWARE_KEYS (true|false) is undefined!")
elseif(BUILD_TARGET_HARDWARE_KEYS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/dummy_crypto_keys.c")
endif()

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver/Driver_MPC.c"
    "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver/Driver_PPC.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver/Driver_USART.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver/Driver_Flash.c")
  # As the SST area is going to be in RAM, it is required to set SST_RAM_FS to be sure the
  # SST service knows that when it starts the SST area does not contain any valid block and
  # it needs  to create an empty one.
  set(SST_RAM_FS True)
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/musca_a/CMSIS_Driver" ABSOLUTE)
endif()
