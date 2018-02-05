/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_PARTITION_DEFS_H__
#define __SPM_PARTITION_DEFS_H__

/* FixMe: allocations to be settled.
 * 8 bits reserved by TFM for secure partition Id in this prototype
 */
#define TFM_SP_BASE 256

/* A reserved partition ID that is used for uninitialised data */
#define INVALID_PARTITION_ID (~0U)

/* ***** partition ID-s internal to the TFM ***** */
/* From the SPM point of view the non secure processing environment is handled
 * as a special secure partition. This simplifies the context switch
 * operations.
 */
#define TFM_SP_NON_SECURE_ID (0)
/* A dummy partition for TFM_SP_CORE is created to handle secure partition
 * calls done directly from the core, before NS execution started.
 */
#define TFM_SP_CORE_ID (1)

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_ID (2)
#define TFM_SP_CORE_TEST_2_ID (3)
#endif /* TFM_PARTITION_TEST_CORE */

/* Give SST test service next ID after core test services */
#ifdef TFM_PARTITION_TEST_SST
#define TFM_SP_SST_TEST_PARTITION_ID (4)
#endif /* TFM_PARTITION_TEST_SST */

/* ***** Normal partition ID-s ***** */
#define TFM_SP_STORAGE_ID (TFM_SP_BASE)
#define TFM_SP_AUDIT_LOG_ID (TFM_SP_BASE + 1)

#endif /* __SPM_PARTITION_DEFS_H__ */
