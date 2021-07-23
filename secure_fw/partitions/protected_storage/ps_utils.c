/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ps_utils.h"
#include "tfm_memory_utils.h"

psa_status_t ps_utils_check_contained_in(uint32_t superset_size,
                                         uint32_t subset_offset,
                                         uint32_t subset_size)
{
    /* Check that subset_offset is valid */
    if (subset_offset > superset_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that subset_offset + subset_size fits in superset_size.
     * The previous check passed, so we know that subset_offset <= superset_size
     * and so the right hand side of the inequality cannot underflow.
     */
    if (subset_size > (superset_size - subset_offset)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

#ifdef PS_ENCRYPTION
psa_status_t ps_utils_fill_key_label(psa_storage_uid_t uid,
                                     int32_t client_id,
                                     uint8_t *buff,
                                     size_t buff_len)
{

    if (buff_len < (sizeof(client_id) + sizeof(uid))) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (buff == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    tfm_memset(buff, 0x0, buff_len);
    tfm_memcpy(buff, &client_id, sizeof(client_id));
    tfm_memcpy(buff + sizeof(client_id), &uid, sizeof(uid));

    return PSA_SUCCESS;
}
#endif
