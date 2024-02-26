#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Once all options are set, set common options as fallback

set(TFM_ATTESTATION_SCHEME      "DPE"       CACHE STRING    "Attestation scheme to use [OFF, PSA, CCA, DPE]")

include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
