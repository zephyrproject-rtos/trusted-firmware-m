#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

############################ Partitions ########################################

set(TFM_PSA_API          ON          CACHE BOOL      "Use PSA API instead of secure library model")

# Dummy PSA_FRAMEWORK_ISOLATION_LEVEL to let framework_feature.h.in "#define" it always.
# PSA_FRAMEWORK_ISOLATION_LEVEL should replace TFM_ISOLATION_LEVEL in the future.
set(PSA_FRAMEWORK_ISOLATION_LEVEL   ON)
