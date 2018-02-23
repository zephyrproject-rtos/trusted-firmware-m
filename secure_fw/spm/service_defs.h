/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PARTITION_DEFS_H__
#define __PARTITION_DEFS_H__

/* FixMe: allocations to be settled.
 * 8 bits reserved by TFM for secure partition Id this prototype
 */
#define TFM_SP_BASE 256

/* A reserved partition ID that is used for uninitialised data */
#define INVALID_PARITION_ID (~0U)

/* FixMe: current implementation requires consecutive IDs, no gaps */
/* From the SPM point of view the non secure processing environment is handled
 * as a special secure partition. This simplifies the context switch
 * operations.
 */
#define TFM_SP_NON_SECURE_ID (TFM_SP_BASE + 0)
/* A dummy partition for TFM_SP_CORE is created to handle secure partition
 * calls done directly from the core, before NS execution started.
 */
#define TFM_SP_CORE_ID (TFM_SP_BASE + 1)
#define TFM_SP_STORAGE_ID (TFM_SP_BASE + 2)

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_ID (TFM_SP_BASE + 3)
#define TFM_SP_CORE_TEST_2_ID (TFM_SP_BASE + 4)

/* Give SST test service next ID after core test services */
#ifdef TFM_PARTITION_TEST_SST
#define TFM_SP_SST_TEST_PARTITION_ID (TFM_SP_BASE + 5)
#endif

#elif defined(TFM_PARTITION_TEST_SST) /* CORE_TEST_SERVICES */
/* Avoid creating a gap if core test services are not enabled */
#define TFM_SP_SST_TEST_PARTITION_ID (TFM_SP_BASE + 3)
#endif /* CORE_TEST_SERVICES */

#endif /* __PARTITION_DEFS_H__ */
