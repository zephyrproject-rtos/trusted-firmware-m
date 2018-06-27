/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_ASSET_MANAGEMENT_H__
#define __SST_ASSET_MANAGEMENT_H__

#include <stdint.h>
#include "tfm_sst_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: the secure APP ID should not be share with the non-secure code
 *        as it is revealing information about secure code implementation.
 */
#define S_APP_ID 0xF0000000

#define SST_PERM_BYPASS     (1<<3) /*!< Permission check bypassed. Used when
                                    *   secure a secure entity calls as itself
                                    *   (and not on behalf of another NS app)
                                    */
#define SST_PERM_READ       (1<<2) /*!< Apps can read this asset */
#define SST_PERM_WRITE      (1<<1) /*!< Apps can write to this asset */
#define SST_PERM_REFERENCE  (1)    /*!< Apps can access this value's meta */
#define SST_PERM_FORBIDDEN  (0)    /*!< Used when a security
                                    *   violation detected
                                    */

struct sst_asset_perm_t {
    uint32_t app; /*!< Application ID */
    uint8_t perm; /*!< Permissions bitfield */
};

struct sst_asset_policy_t {
    uint32_t type;         /*!< Asset type */
    uint16_t asset_uuid;   /*!< Asset's unique ID */
    uint16_t perms_count;  /*!< Number of permissions owned by this asset */
    uint16_t max_size;     /*!< Policy maximum size fo this asset */
    uint16_t perms_modes_start_idx; /*!< First permission index */
};

/**
 * \brief Initializes the secure storage system
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_prepare(void);

/**
 * \brief Allocates space for the asset, referenced by asset UUID,
 *        without setting any data in the asset.
 *
 * \param[in] app_id      Application ID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] asset_uuid  Asset UUID
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_create(uint32_t app_id, uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token);

/**
 * \brief Gets the asset's info referenced by asset UUID.
 *
 * \param[in]  app_id      Application ID
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] info        Pointer to store the asset's information
 *                         \ref psa_sst_asset_info_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_get_info(uint32_t app_id, uint32_t asset_uuid,
                                   const struct tfm_sst_token_t *s_token,
                                   struct psa_sst_asset_info_t *info);

/**
 * \brief Gets the asset's attributes referenced by asset UUID.
 *
 * \param[in]  app_id      Application ID
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] attrs       Pointer to store the asset's attributes
 *                         \ref psa_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_get_attributes(uint32_t app_id, uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token,
                                         struct psa_sst_asset_attrs_t *attrs);

/**
 * \brief Sets the asset's attributes referenced by asset UUID.
 *
 * \param[in] app_id      Application ID
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] attrs       Pointer to new the asset's attributes
 *                        \ref psa_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_set_attributes(uint32_t app_id, uint32_t asset_uuid,
                                     const struct tfm_sst_token_t *s_token,
                                     const struct psa_sst_asset_attrs_t *attrs);

/**
 * \brief Reads asset's data referenced by asset UUID.
 *
 * \param[in]  app_id      Application ID
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] data        Pointer to data vector \ref tfm_sst_buf_t to store
 *                         data, size and offset
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_read(uint32_t app_id, uint32_t asset_uuid,
                               const struct tfm_sst_token_t *s_token,
                               struct tfm_sst_buf_t *data);

/**
 * \brief Writes data into an asset referenced by asset UUID.
 *
 * \param[in] app_id      Application ID
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] data        Pointer to data vector \ref tfm_sst_buf_t which
 *                        contains the data to write
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_write(uint32_t app_id, uint32_t asset_uuid,
                                const struct tfm_sst_token_t *s_token,
                                const struct tfm_sst_buf_t *data);

/**
 * \brief Deletes the asset referenced by the asset UUID.
 *
 * \param[in] app_id      Application ID
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_delete(uint32_t app_id, uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token);

#ifdef __cplusplus
}
#endif

#endif /* __SST_ASSET_MANAGEMENT_H__ */
