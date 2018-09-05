/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**********************************************/
/* Automatically-generated file. Do not edit! */
/**********************************************/

#include "sst_asset_defs.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"

const struct sst_asset_policy_t asset_perms[] = {
{
    .type = PSA_SST_ASSET_KEY_AES,
    .asset_uuid = SST_ASSET_ID_AES_KEY_128,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_128,
    .perms_count = 1,
    .perms_modes_start_idx = 0,
},{
    .type = PSA_SST_ASSET_KEY_AES,
    .asset_uuid = SST_ASSET_ID_AES_KEY_192,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_192,
    .perms_count = 3,
    .perms_modes_start_idx = 1,
},{
    .type = PSA_SST_ASSET_KEY_AES,
    .asset_uuid = SST_ASSET_ID_AES_KEY_256,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_256,
    .perms_count = 1,
    .perms_modes_start_idx = 4,
},{
    .type = PSA_SST_ASSET_KEY_RSA_PUBLIC,
    .asset_uuid = SST_ASSET_ID_RSA_KEY_1024,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_1024,
    .perms_count = 1,
    .perms_modes_start_idx = 5,
},{
    .type = PSA_SST_ASSET_KEY_RSA_PUBLIC,
    .asset_uuid = SST_ASSET_ID_RSA_KEY_2048,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_2048,
    .perms_count = 1,
    .perms_modes_start_idx = 6,
},{
    .type = PSA_SST_ASSET_KEY_RSA_PRIVATE,
    .asset_uuid = SST_ASSET_ID_RSA_KEY_4096,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_4096,
    .perms_count = 1,
    .perms_modes_start_idx = 7,
},{
    .type = PSA_SST_ASSET_CERT_X_509,
    .asset_uuid = SST_ASSET_ID_X509_CERT_SMALL,
    .max_size = SST_ASSET_MAX_SIZE_X509_CERT_SMALL,
    .perms_count = 1,
    .perms_modes_start_idx = 8,
},{
    .type = PSA_SST_ASSET_CERT_X_509,
    .asset_uuid = SST_ASSET_ID_X509_CERT_LARGE,
    .max_size = SST_ASSET_MAX_SIZE_X509_CERT_LARGE,
    .perms_count = 3,
    .perms_modes_start_idx = 9,
},{
    .type = PSA_SST_ASSET_KEY_HMAC,
    .asset_uuid = SST_ASSET_ID_SHA224_HASH,
    .max_size = SST_ASSET_MAX_SIZE_SHA224_HASH,
    .perms_count = 1,
    .perms_modes_start_idx = 12,
},{
    .type = PSA_SST_ASSET_KEY_HMAC,
    .asset_uuid = SST_ASSET_ID_SHA384_HASH,
    .max_size = SST_ASSET_MAX_SIZE_SHA384_HASH,
    .perms_count = 1,
    .perms_modes_start_idx = 13,
} };

const struct sst_asset_perm_t asset_perms_modes[] = {
{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE,
},{
    .client_id = SST_CLIENT_ID_2,
    .perm = SST_PERM_REFERENCE,
},{
    .client_id = SST_CLIENT_ID_1,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ,
},{
    .client_id = SST_CLIENT_ID_0,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_2,
    .perm = SST_PERM_REFERENCE,
},{
    .client_id = SST_CLIENT_ID_1,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ,
},{
    .client_id = SST_CLIENT_ID_0,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_1,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
},{
    .client_id = SST_CLIENT_ID_0,
    .perm = SST_PERM_REFERENCE | SST_PERM_WRITE,
} };
