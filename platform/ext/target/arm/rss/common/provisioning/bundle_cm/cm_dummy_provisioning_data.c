/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_provisioning_bundle.h"

const struct cm_provisioning_data data = {
    /* BL1_2_IMAGE_HASH */
    {},
    /* BL1_2_IMAGE */
    {},
    /* DMA ICS */
    {},
    /* SCP data */
    {},
    /* RSS_ID */
    0,
    /* SAM config placeholder */
    {
        0xFFFFFFFF, 0x0000001F,
        0xF99FFFFF, 0x0000001F,
        0x88888828, 0x88888888,
        0x80000088, 0x88888008,
        0x00088888, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000,

        /* This is the integrity check value, it must equal the zero count of
         * the rest of the SAM config.
         */
        0x0000027C,
    },
    /* GUK */
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45,
        0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 0x01,
        0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67,
        0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23,
    },
    /* CCA system properties placeholder */
    0xDEADBEEF,
};
