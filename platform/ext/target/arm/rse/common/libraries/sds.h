/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SDS_API_H__
#define __SDS_API_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "tfm_plat_defs.h"

struct sds_structure_desc {
    /*! Identifier of the structure to be created. */
    uint32_t id;

    /*! Size, in bytes, of the structure. */
    size_t size;

    /*!
     *  Payload of the structure. If not equal to NULL, as part of the
     *  initialization of the module's elements, the payload of the structure
     *  identified by 'id' is initialized/updated to the value pointed to by
     *  'payload'.
     */
    const void *payload;

    /*! Set the valid flag in the structure if true. */
    bool finalize;
};

enum tfm_plat_err_t sds_struct_add(const struct sds_structure_desc *struct_desc);

#endif /* __SDS_API_H__ */
