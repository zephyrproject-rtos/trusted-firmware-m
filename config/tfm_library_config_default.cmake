#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

############################ Partitions ########################################

set(TFM_PSA_API                 OFF          CACHE BOOL      "Use PSA API instead of secure library model")
set(TFM_PARTITION_AUDIT_LOG     ON           CACHE BOOL      "Enable Audit Log partition")
set(CONFIG_TFM_PARTITION_META   OFF)
set(TFM_PARTITION_NS_AGENT_MAILBOX OFF)
set(TFM_PARTITION_NS_AGENT_TZ OFF)
