#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

############################ Partitions ########################################

set(TFM_PARTITION_AUDIT_LOG             OFF          CACHE BOOL      "Enable Audit Log partition")
set(TFM_PARTITION_FIRMWARE_UPDATE       OFF          CACHE BOOL      "Enable firmware update partition")
set(MCUBOOT_DATA_SHARING                OFF          CACHE BOOL      "Add sharing of application specific data using the same shared data area as for the measured boot")
