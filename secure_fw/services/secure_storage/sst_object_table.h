/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_OBJECT_TABLE_H__
#define __SST_OBJECT_TABLE_H__

#include <stdint.h>
#include "tfm_sst_defs.h"
#include "crypto/sst_crypto_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \struct sst_obj_table_info_t
 *
 * \brief Object table information structure.
 */
struct sst_obj_table_info_t {
    uint32_t fid;      /*!< File ID in the file system */
#ifdef SST_ENCRYPTION
    uint8_t *tag;      /*!< Pointer to the MAC value of AEAD object */
#else
    uint32_t version;  /*!< Object version */
#endif
};

/**
 * \brief Creates object table.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_create(void);

/**
 * \brief Initializes object table.
 *
 * \param[in/out] obj_data  Pointer to the static object data allocated
 *                          in other to reuse that memory to allocated a
 *                          temporary object table.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_init(uint8_t *obj_data);

/**
 * \brief Checks if there is a table entry based on the given object UUID.
 *
 * \param[in] uuid  Object UUID
 *
 * \return Returns TFM_SST_ERR_SUCCESS if the object exists. Otherwise, it
 *         returns TFM_SST_ERR_ASSET_NOT_FOUND.
 */
enum psa_sst_err_t sst_object_table_obj_exist(uint32_t uuid);

/**
 * \brief Gets a not in use file ID.
 *
 * \param[out] p_fid  Pointer to the location to store the file ID
 *
 * \return Returns TFM_SST_ERR_SUCCESS if the fid is valid. Otherwise, it
 *         returns an error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_get_free_fid(uint32_t *p_fid);

/**
 * \brief Sets object's internal information in the object table and
 *        stores it persitently.
 *
 * \param[in] uuid          Object UUID
 * \param[in] obj_tbl_info  Pointer to the location to store object table
 *                          information \ref sst_obj_table_info_t
 *
 * \note  A call to this function ends up into write the table in the
 *        file system.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_set_obj_tbl_info(uint32_t uuid,
                               const struct sst_obj_table_info_t *obj_tbl_info);

/**
 * \brief Gets object's version based on the given object UUID.
 *
 * \param[in]  uuid          Object UUID
 * \param[out] obj_tbl_info  Pointer to the location to store object table
 *                           information
 *
 * \return Returns TFM_SST_ERR_SUCCESS if the object exists. Otherwise, it
 *         returns TFM_SST_ERR_ASSET_NOT_FOUND.
 */
enum psa_sst_err_t sst_object_table_get_obj_tbl_info(uint32_t uuid,
                                     struct sst_obj_table_info_t *obj_tbl_info);

/**
 * \brief Deletes table's entry based on the given object UUID.
 *
 * \param[in] uuid  Object UUID
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_delete_object(uint32_t uuid);

/**
 * \brief Deletes old object table from the persistent area.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_object_table_delete_old_table(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_OBJECT_TABLE_H__ */
