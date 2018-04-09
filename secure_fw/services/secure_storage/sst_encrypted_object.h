/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_ENCRYPTED_OBJECT_H__
#define __SST_ENCRYPTED_OBJECT_H__

#include <stdint.h>
#include "sst_object_defs.h"
#include "tfm_sst_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Creates a new encrypted object.
 *
 * \param[in/out] obj Pointer to the object structure to be created.
 *
 * Note: The function will use obj to store the encrypted data before write it
 *       into the flash to reduce the memory requirements and the number of
 *       internal copies. So, this object will contain the encrypted object
 *       stored in the flash.
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_encrypted_object_create(struct sst_object_t *obj);

/**
 * \brief Reads object referenced by the object handle
 *
 * \param[in]  object_handle Object handle
 * \param[out] obj           Pointer to the object structure to fill in
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_encrypted_object_read(uint32_t object_handle,
                                             struct sst_object_t *obj);

/**
 * \brief Writes object referenced by the object handle.
 *
 * \param[in]     object_handle Object handle
 * \param[in/out] obj           Pointer to the object structure to write.
 *
 * Note: The function will use obj to store the encrypted data before write it
 *       into the flash to reduce the memory requirements and the number of
 *       internal copies. So, this object will contain the encrypted object
 *       stored in the flash.
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_encrypted_object_write(uint32_t object_handle,
                                              struct sst_object_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* __SST_ENCRYPTED_OBJECT_H__ */
