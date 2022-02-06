# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

IS_UTF_8 = ['SW_COMPONENT_TYPE', 'SW_COMPONENT_VERSION',
            'MEASUREMENT_DESCRIPTION', 'ORIGNATOR', 'PROFILE_ID']

HASH_SIZES = [32, 48, 64]


# Security Lifecycle claims
SL_UNKNOWN = 0x1000
SL_PSA_ROT_PROVISIONING = 0x2000
SL_SECURED = 0x3000
SL_NON_PSA_ROT_DEBUG = 0x4000
SL_RECOVERABLE_PSA_ROT_DEBUG = 0x5000
SL_PSA_LIFECYCLE_DECOMMISSIONED = 0x6000


SL_NAMES = [
    'SL_UNKNOWN',
    'SL_PSA_ROT_PROVISIONING',
    'SL_SECURED',
    'SL_NON_PSA_ROT_DEBUG',
    'SL_RECOVERABLE_PSA_ROT_DEBUG',
    'SL_PSA_LIFECYCLE_DECOMMISSIONED',
]

SL_SHIFT = 12
