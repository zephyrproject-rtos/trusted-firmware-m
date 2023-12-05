/*
 * Copyright (c) 2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_misc.c
 *
 * This file contains the implementation of the error translation function from CC
 * to PSA status codes
 */

#include "cc3xx_misc.h"

psa_status_t cc3xx_to_psa_err(enum cc3xx_error err)
{
    switch (err) {
    case CC3XX_ERR_SUCCESS:
        return PSA_SUCCESS;
    case CC3XX_ERR_FAULT_DETECTED:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case CC3XX_ERR_BUFFER_OVERFLOW:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case CC3XX_ERR_INVALID_LCS:
        return PSA_ERROR_DATA_INVALID;
    case CC3XX_ERR_INVALID_DATA:
        return PSA_ERROR_DATA_INVALID;
    case CC3XX_ERR_INVALID_STATE:
        return PSA_ERROR_BAD_STATE;
    case CC3XX_ERR_NOT_IMPLEMENTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case CC3XX_ERR_ENGINE_IN_USE:
        return PSA_ERROR_GENERIC_ERROR;
    case CC3XX_ERR_KEY_IMPORT_FAILED:
        return PSA_ERROR_NOT_PERMITTED;
    case CC3XX_ERR_INVALID_REMAP_REGION:
        return PSA_ERROR_INVALID_ARGUMENT;
    case CC3XX_ERR_BUS_ERROR:
        return PSA_ERROR_COMMUNICATION_FAILURE;
    case CC3XX_ERR_RNG_TOO_MANY_ATTEMPTS:
        return PSA_ERROR_GENERIC_ERROR;
    case CC3XX_ERR_VERIFY_FAILED:
        return PSA_ERROR_INVALID_SIGNATURE;
    case CC3XX_ERR_INVALID_TAG:
        return PSA_ERROR_INVALID_SIGNATURE;
    case CC3XX_ERR_DMA_OUTPUT_BUFFER_TOO_SMALL:
        return PSA_ERROR_BUFFER_TOO_SMALL;
    case CC3XX_ERR_INVALID_INPUT_LENGTH:
        return PSA_ERROR_INVALID_ARGUMENT;
    case CC3XX_ERR_GCM_VARIABLE_IV_NOT_IMPLEMENTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case CC3XX_ERR_CHACHA_IV_SIZE_INCORRECT:
        return PSA_ERROR_INVALID_ARGUMENT;
    case CC3XX_ERR_DFA_VIOLATION:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case CC3XX_ERR_DRBG_RESEED_REQUIRED:
        return PSA_ERROR_BAD_STATE;
    case CC3XX_ERR_DRBG_INVALID_ID:
        return PSA_ERROR_INVALID_ARGUMENT;
    default:
        return PSA_ERROR_HARDWARE_FAILURE;
    }
}
