/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_PRIV_ASSERT_H__
#define __TFM_PRIV_ASSERT_H__

#include <string.h>
#include "tfm_spm_log.h"

#ifndef NDEBUG
#define SPM_ASSERT(cond)                                \
            do {                                        \
                if (!(cond)) {                          \
                    SPMLOG_INFMSG("Assert:");           \
                    SPMLOG_INFMSG(__func__);            \
                    SPMLOG_INFMSGVAL(",", __LINE__);    \
                    while (1) {                         \
                        ;                               \
                    }                                   \
                }                                       \
            } while (0)
#else
#define SPM_ASSERT(cond)
#endif

#define assert(cond) SPM_ASSERT(cond)

#endif /* __TFM_PRIV_ASSERT_H__ */
