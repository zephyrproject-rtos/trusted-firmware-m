/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_asset_defs.h"

struct sst_asset_info_t asset_perms[] = {
{
    .asset_uuid = SST_ASSET_ID_AES_KEY_128,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_128,
    .perms_count = SST_ASSET_PERMS_COUNT_AES_KEY_128,
    .perms_modes_start_idx = 0,
}, {
    .asset_uuid = SST_ASSET_ID_AES_KEY_192,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_192,
    .perms_count = SST_ASSET_PERMS_COUNT_AES_KEY_192,
    .perms_modes_start_idx = 1,
}, {
    .asset_uuid = SST_ASSET_ID_AES_KEY_256,
    .max_size = SST_ASSET_MAX_SIZE_AES_KEY_256,
    .perms_count = SST_ASSET_PERMS_COUNT_AES_KEY_256,
    .perms_modes_start_idx = 2,
}, {
    .asset_uuid = SST_ASSET_ID_RSA_KEY_1024,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_1024,
    .perms_count = SST_ASSET_PERMS_COUNT_RSA_KEY_1024,
    .perms_modes_start_idx = 3,
}, {
    .asset_uuid = SST_ASSET_ID_RSA_KEY_2048,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_2048,
    .perms_count = SST_ASSET_PERMS_COUNT_RSA_KEY_2048,
    .perms_modes_start_idx = 4,
}, {
    .asset_uuid = SST_ASSET_ID_RSA_KEY_4096,
    .max_size = SST_ASSET_MAX_SIZE_RSA_KEY_4096,
    .perms_count = SST_ASSET_PERMS_COUNT_RSA_KEY_4096,
    .perms_modes_start_idx = 5,
}, {
    .asset_uuid = SST_ASSET_ID_X509_CERT_SMALL,
    .max_size = SST_ASSET_MAX_SIZE_X509_CERT_SMALL,
    .perms_count = SST_ASSET_PERMS_COUNT_X509_CERT_SMALL,
    .perms_modes_start_idx = 6,
}, {
/* The following assets are used by the SST testsuite */
    .asset_uuid = SST_ASSET_ID_X509_CERT_LARGE,
    .max_size = SST_ASSET_MAX_SIZE_X509_CERT_LARGE,
    .perms_count = SST_ASSET_PERMS_COUNT_X509_CERT_LARGE,
    .perms_modes_start_idx = 7,
}, {
    .asset_uuid = SST_ASSET_ID_SHA224_HASH,
    .max_size = SST_ASSET_MAX_SIZE_SHA224_HASH,
    .perms_count = SST_ASSET_PERMS_COUNT_SHA224_HASH,
    .perms_modes_start_idx = 10,
}, {
    .asset_uuid = SST_ASSET_ID_SHA384_HASH,
    .max_size = SST_ASSET_MAX_SIZE_SHA384_HASH,
    .perms_count = SST_ASSET_PERMS_COUNT_SHA384_HASH,
    .perms_modes_start_idx = 11,
}, };

struct sst_asset_perm_t asset_perms_modes[] = {
{
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_3,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
/* The following asset permissions are used by the SST testsuite */
    .app = SST_APP_ID_0,
    .perm = SST_PERM_REFERENCE,
}, {
    .app = SST_APP_ID_1,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ,
}, {
    .app = SST_APP_ID_2,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_1,
    .perm = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE,
}, {
    .app = SST_APP_ID_2,
    .perm = SST_PERM_REFERENCE | SST_PERM_WRITE,
}, };
