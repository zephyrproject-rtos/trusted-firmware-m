#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# This is a reference configuration extension file for Profile Medium to
# disable Protected Storage service on devices which don't integrate an
# off-chip storage device.

# Disable Protected Storage service
set(TFM_PARTITION_PROTECTED_STORAGE OFF)
