/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SIC_BOOT_H__
#define __SIC_BOOT_H__

#include <stdint.h>

#include "rse_kmu_slot_ids.h"
#include "boot_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIC_BOOT_ERR_BASE
#define SIC_BOOT_ERR_BASE   0x40000000
#endif /* !SIC_BOOT_ERR_BASE */

enum sic_boot_err_t
{
    SIC_BOOT_SUCCESS = 0,
    SIC_BOOT_INVALID_REGION = (SIC_BOOT_ERR_BASE + 0x01),
    SIC_BOOT_INVALID_ALIGNMENT,
    SIC_BOOT_ERR_AUTH_INIT,
    SIC_BOOT_ERR_AUTH_SETUP,
    SIC_BOOT_ERR_DECR_INIT,
    SIC_BOOT_ERR_DECR_SETUP,
    SIC_BOOT_ERR_DECRKEY_EX,
    SIC_BOOT_ERR_ENABLE,
};

/**
 * \brief                  Initialise Secure I-Cache, and begin configuration.
 *
 * \return                 sic_boot_err_t
 */
enum sic_boot_err_t sic_boot_init(void);

/**
 * \brief               Setup SIC Authentication and Decryption engines for a
 *                      firmware image in SIC address space.
 * \note                Assumes that the XIP firmware image is already mapped
 *                      to SIC region by the ATU.
 * \param[in] sictbl    XIP table address.
 * \param[in] img_addr  Address of firmware image in Secure I-Cache region.
 * \param[in] img_size  Size of firmware image in Secure I-Cache region.
 * \param[in] key       Key slot to use for decryption \ref{rse_kmu_slot_ids.h}.
 * \return              sic_boot_err_t
 */
enum sic_boot_err_t sic_boot_setup_auth_and_decrypt(uintptr_t sictbl,
                                    uintptr_t img_addr,
                                    size_t img_size,
                                    uint8_t region_idx,
                                    enum rse_kmu_slot_id_t key);

/**
 * \brief           Enable Secure I-cache Athentication and Decryption engine.
 *
 * \return          sic_boot_err_t
 */
enum sic_boot_err_t sic_boot_enable_auth_and_decrypt(void);

#ifdef RSE_USE_HOST_FLASH /* unchanged for compatibility */

/**
 * \brief                        Perform post-image-load steps to setup SIC,
 *                               for a given image.
 *
 * \param[in]  image_id          The image id to setup the SIC for.
 * \param[in]  image_load_offset The flash offset the image was loaded from.
 *                               This is used to detect which of the primary /
 *                               secondary images was loaded and determine which
 *                               code should be run through the SIC.
 *
 * \return                       sic_boot_err_t
 */
enum sic_boot_err_t sic_boot_post_load(uint32_t image_id, uint32_t image_load_offset);

/**
 * \brief                  Perform SIC configuration that needs to be run just
 *                         before the bootloader is exited to jump to the new
 *                         image.
 *
 * \param[out] vt_cpy      A pointer to the vector table pointer that is to be
 *                         jumped to. This is updated to the correct address
 *                         where the image can be executed via the SIC.
 *
 * \return                 sic_boot_err_t
 */
enum sic_boot_err_t sic_boot_pre_quit(struct boot_arm_vector_table **vt_cpy);

#endif /* RSE_USE_HOST_FLASH */

#ifdef __cplusplus
}
#endif

#endif /* __SIC_BOOT_H__ */
