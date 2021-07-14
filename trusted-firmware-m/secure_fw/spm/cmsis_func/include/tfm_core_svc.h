/*
 * Copyright (c) 2017-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_SVC_H__
#define __TFM_CORE_SVC_H__

#include "tfm_spm_log.h"

/* SVC numbers */
#define TFM_SVC_PSA_WAIT                (0x0)
#define TFM_SVC_PSA_EOI                 (0x1)
#define TFM_SVC_SFN_REQUEST             (0x2)
#define TFM_SVC_SFN_RETURN              (0x3)
#define TFM_SVC_SFN_COMPLETION          (0x4)
#define TFM_SVC_SPM_REQUEST             (0x5)
#define TFM_SVC_GET_BOOT_DATA           (0x6)
#define TFM_SVC_DEPRIV_REQ              (0x7)
#define TFM_SVC_DEPRIV_RET              (0x8)
#define TFM_SVC_ENABLE_IRQ              (0x9)
#define TFM_SVC_DISABLE_IRQ             (0xA)
#if (TFM_SPM_LOG_LEVEL > TFM_SPM_LOG_LEVEL_SILENCE)
#define TFM_SVC_OUTPUT_UNPRIV_STRING    (0xB)
#endif

#endif /* __TFM_CORE_SVC_H__ */
