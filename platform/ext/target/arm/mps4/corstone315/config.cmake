#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(${MPS4_COMMON_DIR}/config.cmake)

# Ethos-U NPU configurations
set(ETHOSU_ARCH                       "U65"            CACHE STRING    "Ethos-U NPU type [U65]")
