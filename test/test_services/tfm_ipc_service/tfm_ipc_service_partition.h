/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_IPC_SERVICE_PARTITION_H__
#define __TFM_IPC_SERVICE_PARTITION_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API
#define IPC_BASIC_SIGNAL (1 << (0 + 4))
#endif /* TFM_PSA_API */

#endif /* __TFM_IPC_SERVICE_PARTITION_H__ */
