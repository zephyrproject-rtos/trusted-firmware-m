#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all MPS2/AN521 specific files in the application.

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/cmsis_core" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/retarget" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/native_drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/partition" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/cmsis_core/system_cmsdk_mps2_an521.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/retarget/platform_retarget_dev.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/common/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/native_drivers/arm_uart_drv.c")

  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mps2/an521/native_drivers/mpc_sie200_drv.c"
              "${PLATFORM_DIR}/target/mps2/an521/native_drivers/ppc_sse200_drv.c"
              )
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/native_drivers/timer_cmsdk/timer_cmsdk.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/native_drivers/timer_cmsdk" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/mps2/an521/armclang/startup_cmsdk_mps2_an521_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/mps2/an521/armclang/startup_cmsdk_mps2_an521_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/mps2/an521/armclang/startup_cmsdk_mps2_an521_bl2.s")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/target_cfg.c")
endif()

if (NOT DEFINED BUILD_TARGET_HARDWARE_KEYS)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_HARDWARE_KEYS (true|false) is undefined!")
elseif(BUILD_TARGET_HARDWARE_KEYS)
  LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/dummy_crypto_keys.c")
endif()

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  LIST(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mps2/an521/cmsis_drivers/Driver_MPC.c"
    "${PLATFORM_DIR}/target/mps2/an521/cmsis_drivers/Driver_PPC.c")
  LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps2/an521/cmsis_drivers/Driver_USART.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2/an521/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/common/flash_memory_mapped.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/common" ABSOLUTE)
endif()
