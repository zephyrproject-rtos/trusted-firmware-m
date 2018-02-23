/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SERVICE_DEFS_H__
#define __SERVICE_DEFS_H__

/* FixMe: allocations to be settled.
 * 8 bits reserved by TFM for secure service Id this prototype
 */
#define TFM_SEC_FUNC_BASE 256

/* A reserved partition ID that is used for uninitialised data */
#define INVALID_PARITION_ID (~0U)

/* FixMe: current implementation requires consecutive IDs, no gaps */
/* From the SPM point of view the non secure processing environment is handled
 * as a special secure partition. This simplifies the context switch
 * operations.
 */
#define TFM_SEC_FUNC_NON_SECURE_ID (TFM_SEC_FUNC_BASE + 0)
/* A dummy partition for TFM_SEC_FUNC_CORE is created to handle secure service
 * calls done directly from the core, before NS execution started.
 */
#define TFM_SEC_FUNC_CORE_ID (TFM_SEC_FUNC_BASE + 1)
#define TFM_SEC_FUNC_STORAGE_ID (TFM_SEC_FUNC_BASE + 2)

#ifdef CORE_TEST_SERVICES
#define TFM_SEC_FUNC_CORE_TEST_ID (TFM_SEC_FUNC_BASE + 3)
#define TFM_SEC_FUNC_CORE_TEST_2_ID (TFM_SEC_FUNC_BASE + 4)

/* Give SST test service next ID after core test services */
#ifdef SST_TEST_SERVICES
#define TFM_SEC_FUNC_SST_TEST_SERVICE_ID (TFM_SEC_FUNC_BASE + 5)
#endif

#elif defined(SST_TEST_SERVICES) /* CORE_TEST_SERVICES */
/* Avoid creating a gap if core test services are not enabled */
#define TFM_SEC_FUNC_SST_TEST_SERVICE_ID (TFM_SEC_FUNC_BASE + 3)
#endif /* CORE_TEST_SERVICES */

#endif /* __SERVICE_DEFS_H__ */
