/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_UTILS_H__
#define __TFM_UTILS_H__

/* CPU spin here */
void tfm_panic(void);

/* Assert and spin */
#define TFM_ASSERT(cond)                                            \
            do {                                                    \
                if (!(cond)) {                                      \
                    printf("Assert:%s:%d", __FUNCTION__, __LINE__); \
                    while (1)                                       \
                        ;                                           \
                }                                                   \
            } while (0)

#endif
