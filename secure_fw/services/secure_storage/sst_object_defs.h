/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_OBJECT_DEFS_H__
#define __SST_OBJECT_DEFS_H__

#include <stdint.h>
#include "assets/sst_asset_defs.h"
#include "psa_sst_asset_defs.h"

#ifdef SST_ENCRYPTION
#include "crypto/sst_crypto_interface.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sst_obj_header_t {
#ifdef SST_ENCRYPTION
    /* Metadata attached as a header to encrypted object data before storage */
    union sst_crypto_t crypto; /*!< Crypto metadata */
#endif
    uint32_t uuid;                     /*!< Asset ID */
    uint32_t version;                  /*!< Asset version */
    struct psa_sst_asset_info_t  info; /*!< Asset information */
    struct psa_sst_asset_attrs_t attr; /*!< Asset attributes */
};

/* The object to be written to the file system below. Made up of the
 * object header and the object data.
 */
struct sst_object_t {
    struct sst_obj_header_t header;   /*!< Object header */
    uint8_t data[SST_MAX_ASSET_SIZE]; /*!< Object data */
};

#define SST_MAX_OBJECT_DATA_SIZE  SST_MAX_ASSET_SIZE
#define SST_OBJECT_HEADER_SIZE    sizeof(struct sst_obj_header_t)
#define SST_MAX_OBJECT_SIZE       sizeof(struct sst_object_t)

#ifdef __cplusplus
}
#endif

#endif /* __SST_OBJECT_DEFS_H__ */
