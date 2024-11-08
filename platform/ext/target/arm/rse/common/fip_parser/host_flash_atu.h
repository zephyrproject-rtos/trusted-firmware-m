/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HOST_FLASH_ATU_H__
#define __HOST_FLASH_ATU_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                       Gets the offsets of the FIPs and metadata in
 *                              host flash. These are parsed from the GPT
 *                              partition list.
 *
 * \param[out] fip_found        Which fips were found. If fip_found[x] is 0,
 *                              then the value of fip_offsets[x] is undefined.
 * \param[out] fip_offsets      The FIP offsets.
 * \param[out] metadata_found   Which metadata were found. If metadata_found[x]
 *                              is 0, then the value of fip_offsets[x] is
 *                              undefined.
 * \param[out] metadata_offsets The metadata offsets.
 *
 * \return                 0 on success, non-zero on failure.
 */
int host_flash_atu_get_fip_and_metadata_offsets(bool fip_found[2],
                                                uint64_t fip_offsets[2],
                                                bool metadata_found[2],
                                                uint64_t metadata_offsets[2]);

/**
 * \brief                             Set up the input ATU slots so that an
 *                                    image can be loaded from host flash.
 *                                    Parses a FIP via an intermediate ATU slot
 *                                    to find the image offset.
 *
 * \param[in] fip_offset              The host flash offset of the FIP that
 *                                    should be parsed to find the image offset.
 *
 * \param[in] slot                    The ATU slot that should be setup as the
 *                                    image input slot.
 *
 * \param[in] logical_address         The address in RSE memory to which the ATU
 *                                    should map the image.
 *
 * \param[in] image_uuid              The UUID of the image that should be have
 *                                    its slot set up. This is used when parsing
 *                                    the FIP for the offset.
 *
 * \param[out] logical_address_offset The offset that the image has been mapped
 *                                    at (so the base of the image is
 *                                    logical_address + logical_address_offset).
 *                                    This is required because images may not be
 *                                    aligned to the ATU page size.
 *
 * \param[out] atu_slot_size          The size of the mapped image.
 *
 * \return                            0 on success, non-zero on failure.
 */
int host_flash_atu_setup_image_input_slots_from_fip(uint64_t fip_offset,
                                                    uint32_t slot,
                                                    uintptr_t logical_address,
                                                    uuid_t image_uuid,
                                                    uint32_t *logical_address_offset,
                                                    size_t *atu_slot_size);

/**
 * \brief                             Setup the input and output slots for a
 *                                    given image. returns the offsets from the
 *                                    expected logical address that the image
 *                                    has been mapped to (to account for images
 *                                    not being aligned to the ATU page size).
 *
 * \param[in] image_uuid              The UUID of the image that should be have
 *                                    its input and output slots set up.
 *
 * \param[out] offsets                The offsets that the primary and secondary
 *                                    images for that particular UUID have been
 *                                    mapped at (offset from their expected
 *                                    logical addresses).
 *
 * \return                            0 on success, non-zero on failure.
 */
int host_flash_atu_init_regions_for_image(uuid_t image_uuid, uint32_t offsets[2]);

/**
 * \brief                             Teardown all image input and output slots.
 *                                    Should be called between mapping different
 *                                    images.
 *
 * \return                            0 on success, non-zero on failure.
 */
int host_flash_atu_uninit_regions(void);

#ifdef __cplusplus
}
#endif

#endif /* __HOST_FLASH_ATU_H__ */
