/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_PROVISIONING_CHECK_CONFIG_H__
#define __RSE_PROVISIONING_CHECK_CONFIG_H__

#if defined(TFM_BL1_2_IMAGE_ENCRYPTION) && !defined(RSE_CM_PROVISION_KCE_CM)
#error TFM_BL1_2_IMAGE_ENCRYPTION requires RSE_CM_PROVISION_KCE_CM
#endif

#endif /* __RSE_PROVISIONING_CHECK_CONFIG_H__ */
