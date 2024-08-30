/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file bl2_image_id.h
 * \brief This file contains the IDs of firmware images loaded by BL2.
 */

#ifndef __BL2_IMAGE_ID_H__
#define __BL2_IMAGE_ID_H__

/*
 * ID of firmware images loaded by BL2
 * Images are loaded by BL2 in reverse order of ID
 * Cannot use enum as this is included in linker files.
 */
/* RSE firmware */
#define RSE_FIRMWARE_SECURE_ID       0
#define RSE_FIRMWARE_NON_SECURE_ID   1
/* Host Firmware */
#define RSE_FIRMWARE_AP_BL2_ID       2
#define RSE_FIRMWARE_SI_CL2_ID       3
#define RSE_FIRMWARE_SI_CL1_ID       4
#define RSE_FIRMWARE_SI_CL0_ID       5
#define RSE_FIRMWARE_SCP_ID          6
/* Number of firmware loaded by BL2 */
#define RSE_FIRMWARE_COUNT           7

#endif /* __BL2_IMAGE_ID_H__ */
