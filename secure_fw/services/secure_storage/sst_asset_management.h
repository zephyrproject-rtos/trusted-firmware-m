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

/* FIXME: the secure client ID should not be share with the non-secure code
 *        as it is revealing information about secure code implementation.
 */
#define S_CLIENT_ID 0x00000001

/* Invalid client ID (CID) */
#define SST_INVALID_CLIENT_ID   0x00000000

/* Non-Secure Processing Environment (NSPE) client ID mask */
#define SST_NSPE_CLIENT_ID_MASK 0x80000000

/**
 * \def SST_IS_CID_NSPE_CID
 *
 * \brief Checks if the client ID is from a non-secure client ID.
 *
 * \param[in] cid  Client ID to check
 *
 * \return Returns 1 if the pid is a non-secure client ID. Otherwise,
 *         it returns 0.
 */
#define SST_IS_CID_NSPE_CID(cid)  ((cid & SST_NSPE_CLIENT_ID_MASK) != 0)


#define SST_PERM_BYPASS     (1<<3) /*!< Permission check bypassed. Used when
                                    *   secure a secure entity calls as itself
                                    *   (and not on behalf of another NS client)
                                    */
#define SST_PERM_READ       (1<<2) /*!< Client can read this asset */
#define SST_PERM_WRITE      (1<<1) /*!< Client can write to this asset */
#define SST_PERM_REFERENCE  (1)    /*!< Client can access this value's meta */
#define SST_PERM_FORBIDDEN  (0)    /*!< Used when a security
                                    *   violation detected
                                    */

struct sst_asset_perm_t {
    int32_t client_id;  /*!< Client ID */
    uint8_t perm;       /*!< Permissions bitfield */
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
 * \param[in] client_id   Client ID which calls the service
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] asset_uuid  Asset UUID
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_create(int32_t client_id,
                                 uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token);

/**
 * \brief Gets the asset's info referenced by asset UUID.
 *
 * \param[in]  client_id   Client ID which calls the service
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] info        Pointer to store the asset's information
 *                         \ref psa_sst_asset_info_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_get_info(int32_t client_id,
                                   uint32_t asset_uuid,
                                   const struct tfm_sst_token_t *s_token,
                                   struct psa_sst_asset_info_t *info);

/**
 * \brief Gets the asset's attributes referenced by asset UUID.
 *
 * \param[in]  client_id   Client ID which calls the service
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] attrs       Pointer to store the asset's attributes
 *                         \ref psa_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_get_attributes(int32_t client_id,
                                         uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token,
                                         struct psa_sst_asset_attrs_t *attrs);

/**
 * \brief Sets the asset's attributes referenced by asset UUID.
 *
 * \param[in] client_id   Client ID which calls the service
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] attrs       Pointer to new the asset's attributes
 *                        \ref psa_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_set_attributes(int32_t client_id,
                                     uint32_t asset_uuid,
                                     const struct tfm_sst_token_t *s_token,
                                     const struct psa_sst_asset_attrs_t *attrs);

/**
 * \brief Reads asset's data referenced by asset UUID.
 *
 * \param[in]  client_id   Client ID which calls the service.
 *                         In case, the caller is a secure partition, this
 *                         parameter can be a non-secure client ID if the
 *                         read is in behalf of that non-secure client ID.
 * \param[in]  asset_uuid  Asset UUID
 * \param[in]  s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[out] data        Pointer to data vector \ref tfm_sst_buf_t to store
 *                         data, size and offset
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_read(int32_t client_id, uint32_t asset_uuid,
                               const struct tfm_sst_token_t *s_token,
                               struct tfm_sst_buf_t *data);

/**
 * \brief Writes data into an asset referenced by asset UUID.
 *
 * \param[in] client_id   Client ID which calls the service
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 * \param[in] data        Pointer to data vector \ref tfm_sst_buf_t which
 *                        contains the data to write
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_write(int32_t client_id, uint32_t asset_uuid,
                                const struct tfm_sst_token_t *s_token,
                                const struct tfm_sst_buf_t *data);

/**
 * \brief Deletes the asset referenced by the asset UUID.
 *
 * \param[in] client_id   Client ID which calls the service
 * \param[in] asset_uuid  Asset UUID
 * \param[in] s_token     Pointer to the asset's token \ref tfm_sst_token_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_am_delete(int32_t client_id, uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token);

#ifdef __cplusplus
}
#endif

#endif /* __SST_ASSET_MANAGEMENT_H__ */
