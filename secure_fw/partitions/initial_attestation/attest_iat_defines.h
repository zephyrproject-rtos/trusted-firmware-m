/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTEST_IAT_DEFINES_H__
#define __ATTEST_IAT_DEFINES_H__

#ifdef __cplusplus
extern "C" {
#endif

#define IAT_ARM_RANGE_BASE                 (-75000)
#define IAT_PROFILE_DEFINITION             (IAT_ARM_RANGE_BASE - 0)
#define IAT_CLIENT_ID                      (IAT_ARM_RANGE_BASE - 1)
#define IAT_SECURITY_LIFECYCLE             (IAT_ARM_RANGE_BASE - 2)
#define IAT_IMPLEMENTATION_ID              (IAT_ARM_RANGE_BASE - 3)
#define IAT_BOOT_SEED                      (IAT_ARM_RANGE_BASE - 4)
#define IAT_HW_VERSION                     (IAT_ARM_RANGE_BASE - 5)
#define IAT_SW_COMPONENTS                  (IAT_ARM_RANGE_BASE - 6)
#define IAT_NO_SW_COMPONENTS               (IAT_ARM_RANGE_BASE - 7)
#define IAT_CHALLENGE                      (IAT_ARM_RANGE_BASE - 8)
#define IAT_UEID                           (IAT_ARM_RANGE_BASE - 9)
#define IAT_ORIGINATION                    (IAT_ARM_RANGE_BASE - 10)

#define IAT_SW_COMPONENT_MEASUREMENT_TYPE  (1)
#define IAT_SW_COMPONENT_MEASUREMENT_VALUE (2)
/* Reserved                                (3) */
#define IAT_SW_COMPONENT_VERSION           (4)
#define IAT_SW_COMPONENT_SIGNER_ID         (5)
#define IAT_SW_COMPONENT_MEASUREMENT_DESC  (6)

/* Indicates that the boot status intentionally (i.e. boot loader does not
 * capable to produce it) does not contain any SW components' measurement.
 * Required integer value for claim labeled IAT_NO_SW_COMPONENTS.
 */
#define NO_SW_COMPONENT_FIXED_VALUE 1

#ifdef __cplusplus
}
#endif

#endif /* __ATTEST_IAT_DEFINES_H__ */
