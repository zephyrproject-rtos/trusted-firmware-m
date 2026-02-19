#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Set platform specific configs that depend on default configs provided by TF-M.

#################################### IFX #######################################

if(IFX_MTB_BUILD AND (NOT TFM_SPM_LOG_RAW_ENABLED) AND (NOT TFM_SP_LOG_RAW_ENABLED))
    # Disable UART for TFM if it's MTB build (MTB NSPEs use own logging method)
    # and both SPM and partition logging are disabled
    set(IFX_UART_ENABLED  OFF  CACHE BOOL  "Enable UART used by SPM log, partition log and/or platform service")
else()
    # Enable UART for TFM if it's not a MTB build (e.g. tests builds, ...) or
    # either SPM or partition logging is enabled
    set(IFX_UART_ENABLED  ON   CACHE BOOL  "Enable UART used by SPM log, partition log and/or platform service")
endif()
