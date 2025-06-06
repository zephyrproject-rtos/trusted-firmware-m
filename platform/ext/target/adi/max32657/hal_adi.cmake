#-------------------------------------------------------------------------------
# Portions Copyright (C) 2025 Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

################################################################################
# Fetch hal_adi repository
set(FETCHCONTENT_QUIET TRUE)

fetch_remote_library(
    LIB_NAME                hal_adi
    LIB_SOURCE_PATH_VAR     HAL_ADI_PATH
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://github.com/analogdevicesinc/hal_adi
        GIT_TAG             ${HAL_ADI_VERSION}
        GIT_PROGRESS        TRUE
)

set(TARGET_LC "max32657"  CACHE STRING "")
string(TOUPPER ${TARGET_LC} TARGET_UC)

set(HAL_ADI_PATH            ${HAL_ADI_PATH}  CACHE PATH "")
set(HAL_ADI_LIBRARY_DIR     ${HAL_ADI_PATH}/MAX/Libraries CACHE PATH   "")

set(HAL_ADI_CMSIS_DIR       ${HAL_ADI_LIBRARY_DIR}/CMSIS/Device/Maxim/${TARGET_UC} CACHE PATH   "")
set(HAL_ADI_CMSIS_INC_DIR   ${HAL_ADI_CMSIS_DIR}/Include  CACHE PATH   "")
set(HAL_ADI_CMSIS_SRC_DIR   ${HAL_ADI_CMSIS_DIR}/Source   CACHE PATH   "")

set(HAL_ADI_PERIPH_DIR      ${HAL_ADI_LIBRARY_DIR}/PeriphDrivers       CACHE PATH   "")
set(HAL_ADI_PERIPH_INC_DIR  ${HAL_ADI_PERIPH_DIR}/Include/${TARGET_UC} CACHE PATH   "")
set(HAL_ADI_PERIPH_SRC_DIR  ${HAL_ADI_PERIPH_DIR}/Source               CACHE PATH   "")


###### BL2 Related Cmake Configurations ########################################
if(BL2)
    target_include_directories(platform_bl2
        PUBLIC
            ${HAL_ADI_PERIPH_INC_DIR}
            ${HAL_ADI_CMSIS_INC_DIR}
    )

    target_sources(platform_bl2
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_common.c
            ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_revb.c
            ${HAL_ADI_PERIPH_SRC_DIR}/SYS/sys_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/SYS/mxc_delay.c
            ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_common.c
            ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_reva.c
            ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_reva.c
            ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_common.c
            ${HAL_ADI_PERIPH_SRC_DIR}/SYS/pins_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/ICC/icc_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/ICC/icc_reva.c

            ${HAL_ADI_PERIPH_SRC_DIR}/DMA/dma_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/DMA/dma_reva.c
    )

    target_compile_definitions(platform_bl2
        PUBLIC
            TARGET=${TARGET_UC}
            TARGET_REV=0x4131
            CONFIG_TRUSTED_EXECUTION_SECURE
            IS_SECURE_ENVIRONMENT

            __MXC_FLASH_MEM_BASE=0x11000000
            __MXC_FLASH_MEM_SIZE=0x00100000
    )
endif() # BL2


###### TFM Related Cmake Configurations ########################################
target_compile_definitions(platform_s
    PUBLIC
        TARGET=${TARGET_UC}
        TARGET_REV=0x4131
        CONFIG_TRUSTED_EXECUTION_SECURE
        IS_SECURE_ENVIRONMENT

        __MXC_FLASH_MEM_BASE=0x11000000
        __MXC_FLASH_MEM_SIZE=0x00100000
)

target_include_directories(platform_s
    PUBLIC
        ${HAL_ADI_PERIPH_INC_DIR}
        ${HAL_ADI_CMSIS_INC_DIR}

        ${HAL_ADI_PERIPH_SRC_DIR}/SYS
        ${HAL_ADI_PERIPH_SRC_DIR}/AES
        ${HAL_ADI_PERIPH_SRC_DIR}/CRC
        ${HAL_ADI_PERIPH_SRC_DIR}/DMA
        ${HAL_ADI_PERIPH_SRC_DIR}/FLC
        ${HAL_ADI_PERIPH_SRC_DIR}/GPIO
        ${HAL_ADI_PERIPH_SRC_DIR}/I3C
        ${HAL_ADI_PERIPH_SRC_DIR}/ICC
        ${HAL_ADI_PERIPH_SRC_DIR}/LP
        ${HAL_ADI_PERIPH_SRC_DIR}/RTC
        ${HAL_ADI_PERIPH_SRC_DIR}/SPI
        ${HAL_ADI_PERIPH_SRC_DIR}/TRNG
        ${HAL_ADI_PERIPH_SRC_DIR}/TMR
        ${HAL_ADI_PERIPH_SRC_DIR}/UART
        ${HAL_ADI_PERIPH_SRC_DIR}/WDT
        ${HAL_ADI_PERIPH_SRC_DIR}/WUT
)

target_sources(platform_s
    PRIVATE
        ${PLATFORM_DIR}/ext/target/adi/${TARGET_LC}/device/src/system_${TARGET_LC}.c

        ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_common.c
        ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/UART/uart_revb.c

        ${HAL_ADI_PERIPH_SRC_DIR}/SYS/sys_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/SYS/mxc_delay.c

        ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_common.c
        ${HAL_ADI_PERIPH_SRC_DIR}/FLC/flc_reva.c

        ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_reva.c
        ${HAL_ADI_PERIPH_SRC_DIR}/GPIO/gpio_common.c

        ${HAL_ADI_PERIPH_SRC_DIR}/SYS/pins_me30.c

        ${HAL_ADI_PERIPH_SRC_DIR}/TZ/spc_me30.c

        ${HAL_ADI_PERIPH_SRC_DIR}/ICC/icc_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/ICC/icc_reva.c

        ${HAL_ADI_PERIPH_SRC_DIR}/DMA/dma_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/DMA/dma_reva.c

        ${HAL_ADI_PERIPH_SRC_DIR}/TRNG/trng_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/TRNG/trng_revb.c

        ${HAL_ADI_PERIPH_SRC_DIR}/AES/aes_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/AES/aes_revb.c

        ${HAL_ADI_PERIPH_SRC_DIR}/CRC/crc_me30.c
        ${HAL_ADI_PERIPH_SRC_DIR}/CRC/crc_reva.c
)

if(NOT ADI_NS_PRPH_I3C)
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/I3C/i3c_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/I3C/i3c_reva.c
    )
endif()

if(NOT ADI_NS_PRPH_SPI)
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/SPI/spi_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/SPI/spi_reva1.c
    )
endif()

if(NOT ADI_NS_PRPH_WDT)
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/WDT/wdt_common.c
            ${HAL_ADI_PERIPH_SRC_DIR}/WDT/wdt_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/WDT/wdt_revb.c
    )
endif()

if(NOT ADI_NS_PRPH_RTC)
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/RTC/rtc_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/RTC/rtc_reva.c
    )
endif()

if(NOT (ADI_NS_PRPH_WUT0 AND ADI_NS_PRPH_WUT1))
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/WUT/wut_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/WUT/wut_reva.c
    )
endif()

if(NOT (    ADI_NS_PRPH_TIMER0
        AND ADI_NS_PRPH_TIMER1
        AND ADI_NS_PRPH_TIMER2
        AND ADI_NS_PRPH_TIMER3
        AND ADI_NS_PRPH_TIMER4
        AND ADI_NS_PRPH_TIMER5
        )
    )
    target_sources(platform_s
        PRIVATE
            ${HAL_ADI_PERIPH_SRC_DIR}/TMR/tmr_common.c
            ${HAL_ADI_PERIPH_SRC_DIR}/TMR/tmr_me30.c
            ${HAL_ADI_PERIPH_SRC_DIR}/TMR/tmr_revb.c
    )
endif()
