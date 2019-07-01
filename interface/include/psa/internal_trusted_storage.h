/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** This file describes the PSA Internal Trusted Storage API
*/

#ifndef PSA_INTERNAL_TRUSTED_STORAGE_H
#define PSA_INTERNAL_TRUSTED_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"
#include "psa/storage_common.h"

#ifdef __cplusplus
extern "C" {
#endif
#define PSA_ITS_API_VERSION_MAJOR  1
#define PSA_ITS_API_VERSION_MINOR  0

// This version of the header file is associated with 1.0 final release.

/**
 * Create a new or modify an existing uid/value pair
 */
psa_status_t psa_its_set(psa_storage_uid_t uid,
                         size_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags);

/**
 * Retrieve data associated with a provided UID
 */
psa_status_t psa_its_get(psa_storage_uid_t uid,
                         size_t data_offset,
                         size_t data_size,
                         void *p_data,
                         size_t *p_data_length);

/**
 * Retrieve the metadata about the provided uid
 */
psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info);

/**
 * Remove the provided key and its associated data from the storage
 */
psa_status_t psa_its_remove(psa_storage_uid_t uid);

#ifdef __cplusplus
}
#endif

#endif // PSA_INTERNAL_TRUSTED_STORAGE_H
