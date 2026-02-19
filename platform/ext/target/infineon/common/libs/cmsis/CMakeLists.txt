#-------------------------------------------------------------------------------
# Copyright (c) 2024-2025 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(NS_BUILD)
    # NS image uses CMSIS library from platform install folder
    set(IFX_CMSIS_LIB_PATH "${PLATFORM_DIR}/ext/cmsis" CACHE PATH "Path to CMSIS library")

    add_library(ifx_cmsis_lib INTERFACE)

    target_include_directories(ifx_cmsis_lib
        INTERFACE
            ${IFX_CMSIS_LIB_PATH}/Include
            ${IFX_CMSIS_LIB_PATH}/Include/m-profile
    )

    target_link_libraries(tfm_api_ns
        INTERFACE
            ifx_cmsis_lib
    )
endif()
