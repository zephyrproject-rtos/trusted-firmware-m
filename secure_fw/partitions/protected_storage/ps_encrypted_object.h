/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2024 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PS_ENCRYPTED_OBJECT_H__
#define __PS_ENCRYPTED_OBJECT_H__

#include <stdint.h>
#include "ps_object_defs.h"
#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Reads object referenced by the object File ID.
 *
 * \param[in]  fid      File ID
 * \param[out] obj      Pointer to the object structure to fill in
 * \param[out] p_blocks Pointer to a counter of decryption blocks used.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t ps_encrypted_object_read(uint32_t fid,
                                      struct ps_object_t *obj,
                                      uint32_t *p_blocks);

/**
 * \brief Creates and writes a new encrypted object based on the given
 *        ps_object_t structure data.
 *
 * \param[in]     fid      File ID
 * \param[in,out] obj      Pointer to the object structure to write.
 *
 * Note: The function will use obj to store the encrypted data before write it
 *       into the flash to reduce the memory requirements and the number of
 *       internal copies. So, this object will contain the encrypted object
 *       stored in the flash.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t ps_encrypted_object_write(uint32_t fid,
                                       struct ps_object_t *obj);

/**
 * \brief Determines the number of encryption blocks that will be used to write
 *        an object of the specified size.
 *
 * \param[in] size     Size in bytes of the object to be written.
 *
 * \return Returns number of blocks
 */
uint32_t ps_encrypted_object_blocks(uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __PS_ENCRYPTED_OBJECT_H__ */
