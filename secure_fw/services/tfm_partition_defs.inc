/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#ifndef __TFM_PARTITION_DEFS_INC__
#define __TFM_PARTITION_DEFS_INC__

#define TFM_SP_STORAGE_ID (TFM_SP_BASE + 0)

#define TFM_SP_AUDIT_LOG_ID (TFM_SP_BASE + 1)

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_ID (TFM_SP_BASE + 2)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_2_ID (TFM_SP_BASE + 3)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
#define TFM_SP_SST_TEST_PARTITION_ID (TFM_SP_BASE + 4)
#endif /* TFM_PARTITION_TEST_SST */

#define TFM_MAX_USER_PARTITIONS (5)

#endif /* __TFM_PARTITION_DEFS_INC__ */
