/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BL2_IMAGE_ID_H__
#define __BL2_IMAGE_ID_H__

enum rse_bl2_image_id_t {
    RSE_BL2_IMAGE_S = 0,
    RSE_BL2_IMAGE_NS,
    RSE_BL2_IMAGE_AP,
    RSE_BL2_IMAGE_SCP,
};

#endif /* __BL2_IMAGE_ID_H__ */
