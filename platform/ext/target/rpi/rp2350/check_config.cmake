#-------------------------------------------------------------------------------
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
#-------------------------------------------------------------------------------

## The platform specific NV counters require OTP usage
tfm_invalid_config((NOT PLATFORM_DEFAULT_OTP) EQUAL PLATFORM_DEFAULT_NV_COUNTERS)
