/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_NV_COUNTER_CONFIG_H__
#define __RSE_NV_COUNTER_CONFIG_H__

#define RSE_NV_COUNTER_BL1_AMOUNT 1

#define RSE_NV_COUNTER_BL2_AMOUNT MCUBOOT_IMAGE_NUMBER

#ifdef TFM_PARTITION_PROTECTED_STORAGE
#define RSE_NV_COUNTER_PS_AMOUNT 3
#else
#define RSE_NV_COUNTER_PS_AMOUNT 0
#endif

#define RSE_NV_COUNTER_HOST_AMOUNT 3

#define RSE_NV_COUNTER_SUBPLATFORM_AMOUNT 0

#define RSE_NV_COUNTER_AMOUNT RSE_NV_COUNTER_BL1_AMOUNT \
                            + RSE_NV_COUNTER_BL2_AMOUNT \
                            + RSE_NV_COUNTER_PS_AMOUNT \
                            + RSE_NV_COUNTER_HOST_AMOUNT \
                            + RSE_NV_COUNTER_SUBPLATFORM_AMOUNT

#endif /* __RSE_NV_COUNTER_CONFIG_H__ */
