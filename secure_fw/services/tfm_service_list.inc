/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#ifndef __TFM_SERVICE_LIST_INC__
#define __TFM_SERVICE_LIST_INC__

/******** TFM_SP_STORAGE ********/

/******** TFM_SP_AUDIT_LOG ********/

/******** TFM_SP_CRYPTO ********/

/******** TFM_SP_PLATFORM ********/

/******** TFM_SP_INITIAL_ATTESTATION ********/

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST ********/
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
/******** TFM_SP_CORE_TEST_2 ********/
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
/******** TFM_SP_SST_TEST_PARTITION ********/
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
/******** TFM_SP_SECURE_TEST_PARTITION ********/
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_SERVICE_TEST ********/
{"IPC_SID_BASIC", TFM_SP_IPC_SERVICE_TEST_ID, IPC_BASIC_SIGNAL, 0x00001000, true, 1, TFM_VERSION_POLICY_STRICT},
#endif /* TFM_PSA_API */

#ifdef TFM_PSA_API
/******** TFM_SP_IPC_CLIENT_TEST ********/
#endif /* TFM_PSA_API */

#endif /* __TFM_SERVICE_LIST_INC__ */
