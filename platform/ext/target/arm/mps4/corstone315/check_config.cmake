#-------------------------------------------------------------------------------
#
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
#-------------------------------------------------------------------------------

include(${MPS4_COMMON_DIR}/check_config.cmake)
tfm_invalid_config(NOT ETHOSU_ARCH MATCHES "U65")
