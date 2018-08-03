/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_API_H__
#define __TFM_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* FixMe: sort out DEBUG compile option and limit return value options
 * on external interfaces */
/* Note:
 * TFM will only return values recognized and parsed by TFM core.
 * Service return codes are not automatically passed on to REE.
 * Any non-zero return value is interpreted as an error that may trigger
 * TEE error handling flow.
 */
enum tfm_status_e
{
    TFM_SUCCESS = 0,
    TFM_PARTITION_PENDED,
    TFM_PARTITION_BUSY,
    TFM_ERROR_PARTITION_ALREADY_PENDED,
    TFM_ERROR_SECURE_DOMAIN_LOCKED,
    TFM_ERROR_INVALID_PARAMETER,
    TFM_ERROR_PARTITION_NON_REENTRANT,
    TFM_ERROR_NS_THREAD_MODE_CALL,
    TFM_ERROR_INVALID_EXC_MODE,
    TFM_SECURE_LOCK_FAILED,
    TFM_SECURE_UNLOCK_FAILED,
    TFM_ERROR_GENERIC = 0x1F,
    TFM_PARTITION_SPECIFIC_ERROR_MIN,
};

//==================== Secure function declarations ==========================//

/**
 * \brief Assign client ID to the current TZ context
 *
 * \param[in]  ns_client_id  The client ID to be assigned to the current
 *                           context
 * \return TFM_SUCCESS if the client ID assigned successfully, an error code
 *         according to \ref tfm_status_e in case of error.
 *
 * \note This function have to be called from handler mode.
 */
enum tfm_status_e tfm_register_client_id (int32_t ns_client_id);

//================ End Secure function declarations ==========================//

#ifdef __cplusplus
}
#endif

#endif /* __TFM_API_H__ */
