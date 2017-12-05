#-------------------------------------------------------------------------------
# Copyright (c) 2017, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all MPS2/SSE2 specific files in the application.

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

set(BOARD_ID "mps2")
set(SUBYSTEM_ID "sse_200")

embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/cmsis_core" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/mps2/specific_drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/retarget" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/native_drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/partition" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED MPS2_SSE200_BUILD_CMSIS_CORE)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_CMSIS_CORE)
	list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/cmsis_core/system_cmsdk_mps2_sse_200.c")
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_RETARGET)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_RETARGET (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_RETARGET)
	list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/retarget/platform_retarget_dev.c")
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_UART_STDOUT)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_UART_STDOUT (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_UART_STDOUT)
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/common/uart_stdout.c")
	embedded_include_directories(PATH "${PLATFORM_DIR}/target/common" ABSOLUTE)
	set(MPS2_SSE200_BUILD_NATIVE_DRIVERS true)
	set(MPS2_SSE200_BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_NATIVE_DRIVERS)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_NATIVE_DRIVERS)
	list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/native_drivers/arm_uart_drv.c")

	list(APPEND ALL_SRC_C_S	"${PLATFORM_DIR}/target/sse_200_mps2/sse_200/native_drivers/mpc_sie200_drv.c"
							"${PLATFORM_DIR}/target/sse_200_mps2/sse_200/native_drivers/ppc_sse200_drv.c"
							)
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_MPS2_TIME)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_MPS2_TIME (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_MPS2_TIME)
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}//target/sse_200_mps2/sse_200/native_drivers/timer_cmsdk/timer_cmsdk.c")
	embedded_include_directories(PATH "${PLATFORM_DIR}//target/sse_200_mps2/sse_200/native_drivers/timer_cmsdk" ABSOLUTE)
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_STARTUP)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_STARTUP (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_STARTUP)
	if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
		list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/armclang/startup_cmsdk_sse_200_s.s")
		list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/armclang/startup_cmsdk_sse_200_ns.s")
	else()
		message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
	endif()
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_TARGET_CFG)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_TARGET_CFG (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_TARGET_CFG)
	list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/target_cfg.c")
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_TARGET_HARDWARE_KEYS)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_TARGET_HARDWARE_KEYS (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_TARGET_HARDWARE_KEYS)
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/dummy_crypto_keys.c")
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_CMSIS_DRIVERS)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_CMSIS_DRIVERS)
	LIST(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/cmsis_drivers/Driver_MPC.c"
		"${PLATFORM_DIR}/target/sse_200_mps2/sse_200/cmsis_drivers/Driver_PPC.c")
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/cmsis_drivers/Driver_USART.c")
	embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/cmsis_drivers" ABSOLUTE)
	embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_MPS2_BOARD_LEDS)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_MPS2_BOARD_LEDS (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_MPS2_BOARD_LEDS)
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board/mps2_leds.c")
	embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board" ABSOLUTE)
endif()

if (NOT DEFINED MPS2_SSE200_BUILD_MPS2_BOARD_TIME)
	message(FATAL_ERROR "Configuration variable MPS2_SSE200_BUILD_MPS2_BOARD_TIME (true|false) is undefined!")
elseif(MPS2_SSE200_BUILD_MPS2_BOARD_TIME)
	LIST(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board/mps2_time.c")
	embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board" ABSOLUTE)
endif()
