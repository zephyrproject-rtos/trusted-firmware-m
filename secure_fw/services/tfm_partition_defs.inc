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

#define TFM_SP_CRYPTO_ID (TFM_SP_BASE + 2)

#define TFM_SP_PLATFORM_ID (TFM_SP_BASE + 3)

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_ID (TFM_SP_BASE + 4)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_SP_CORE_TEST_2_ID (TFM_SP_BASE + 5)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
#define TFM_SP_SST_TEST_PARTITION_ID (TFM_SP_BASE + 6)
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
#define TFM_SP_SECURE_TEST_PARTITION_ID (TFM_SP_BASE + 7)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#define TFM_MAX_USER_PARTITIONS (8)

#endif /* __TFM_PARTITION_DEFS_INC__ */
