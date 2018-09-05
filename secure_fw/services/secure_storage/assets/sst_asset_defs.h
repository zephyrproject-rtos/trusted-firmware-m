/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**********************************************/
/* Automatically-generated file. Do not edit! */
/**********************************************/

#ifndef __SST_ASSET_DEFS_H__
#define __SST_ASSET_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* SST service reserved IDs */
#define SST_ASSET_ID_NO_ASSET 0
/* End SST service reserved IDs */

/* Asset IDs */
#define SST_ASSET_ID_AES_KEY_128 1
#define SST_ASSET_ID_AES_KEY_192 2
#define SST_ASSET_ID_AES_KEY_256 3
#define SST_ASSET_ID_RSA_KEY_1024 4
#define SST_ASSET_ID_RSA_KEY_2048 5
#define SST_ASSET_ID_RSA_KEY_4096 6
#define SST_ASSET_ID_X509_CERT_SMALL 7
#define SST_ASSET_ID_X509_CERT_LARGE 8
#define SST_ASSET_ID_SHA224_HASH 9
#define SST_ASSET_ID_SHA384_HASH 10

/* Asset sizes */
#define SST_ASSET_MAX_SIZE_AES_KEY_128 16
#define SST_ASSET_MAX_SIZE_AES_KEY_192 24
#define SST_ASSET_MAX_SIZE_AES_KEY_256 32
#define SST_ASSET_MAX_SIZE_RSA_KEY_1024 128
#define SST_ASSET_MAX_SIZE_RSA_KEY_2048 256
#define SST_ASSET_MAX_SIZE_RSA_KEY_4096 512
#define SST_ASSET_MAX_SIZE_X509_CERT_SMALL 512
#define SST_ASSET_MAX_SIZE_X509_CERT_LARGE 2048
#define SST_ASSET_MAX_SIZE_SHA224_HASH 28
#define SST_ASSET_MAX_SIZE_SHA384_HASH 48

/* Client IDs which have access rights in one or more assets */
#define SST_CLIENT_ID_0 -1
#define SST_CLIENT_ID_1 -2
#define SST_CLIENT_ID_2 -3
#define SST_CLIENT_ID_3 -4

/* Number of assets that can be stored in SST area */
#define SST_NUM_ASSETS 10

/* Largest defined asset size */
#define SST_MAX_ASSET_SIZE 2048

#ifdef __cplusplus
}
#endif

#endif /* __SST_ASSET_DEFS_H__ */
