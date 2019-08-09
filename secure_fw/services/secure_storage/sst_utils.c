/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_utils.h"

psa_ps_status_t sst_utils_check_contained_in(uint32_t superset_size,
                                             uint32_t subset_offset,
                                             uint32_t subset_size)
{
    /* Check that subset_offset is valid */
    if (subset_offset > superset_size) {
        return PSA_PS_ERROR_OFFSET_INVALID;
    }

    /* Check that subset_offset + subset_size fits in superset_size.
     * The previous check passed, so we know that subset_offset <= superset_size
     * and so the right hand side of the inequality cannot underflow.
     */
    if (subset_size > (superset_size - subset_offset)) {
        return PSA_PS_ERROR_INCORRECT_SIZE;
    }

    return PSA_PS_SUCCESS;
}

psa_ps_status_t psa_status_to_psa_ps_status(psa_status_t status)
{
    switch (status) {
    case PSA_SUCCESS:
        return PSA_PS_SUCCESS;
    case PSA_ERROR_NOT_PERMITTED:
        return PSA_PS_ERROR_WRITE_ONCE;
    case PSA_ERROR_NOT_SUPPORTED:
        return PSA_PS_ERROR_FLAGS_NOT_SUPPORTED;
    case PSA_ERROR_INSUFFICIENT_STORAGE:
        return PSA_PS_ERROR_INSUFFICIENT_SPACE;
    case PSA_ERROR_STORAGE_FAILURE:
        return PSA_PS_ERROR_STORAGE_FAILURE;
    case PSA_ERROR_DOES_NOT_EXIST:
        return PSA_PS_ERROR_UID_NOT_FOUND;
    case PSA_ERROR_INVALID_ARGUMENT:
        return PSA_PS_ERROR_INVALID_ARGUMENT;
    default:
        return PSA_PS_ERROR_OPERATION_FAILED;
    }
}
