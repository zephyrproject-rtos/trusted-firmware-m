/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_DEVICE_ID_H__
#define __TFM_PLAT_DEVICE_ID_H__
/**
 * \file tfm_plat_device_id.h
 *
 * The interfaces defined in this file are meant to provide the following
 * attributes of the device:
 *  - Instance ID:       Unique identifier of the device.
 *  - Implementation ID: Original implementation signer of the attestation key.
 */

/**
 * \note The interfaces defined in this file must be implemented for each
 *       SoC.
 */

#include <stdint.h>
#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def INSTANCE_ID_MAX_SIZE
 *
 * \brief Maximum size of instance ID in bytes
 */
#define INSTANCE_ID_MAX_SIZE (33u)

/**
 * \def IMPLEMENTATION_ID_MAX_SIZE
 *
 * \brief Maximum size of implementation ID in bytes
 */
#define IMPLEMENTATION_ID_MAX_SIZE (32u)

/**
 * \brief Get the UEID of the device.
 *
 * This mandatory claim represents the unique identifier of the instance.
 * In the PSA definition is a hash of the public attestation key of the
 * instance. The claim will be represented by the EAT standard claim UEID
 * of type GUID. The EAT definition of a GUID type is that it will be between
 * 128 & 256 bits but this implementation will use the full 256 bits to
 * accommodate a hash result.
 *
 * \param[in/out] size  As an input value it indicates the size of the caller
 *                      allocated buffer (in bytes) to store the UEID. At return
 *                      its value is updated with the exact size of the UEID.
 * \param[out]    buf   Pointer to the buffer to store the UEID
 *
 * \return  Returns error code specified in \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_plat_get_instance_id(uint32_t *size, uint8_t *buf);

/**
 * \brief Get the Implementation ID of the device.
 *
 * This mandatory claim represents the original implementation signer of the
 * attestation key and identifies the contract between the report and
 * verification. A verification service will use this claim to locate the
 * details of the verification process. The claim will be represented by a
 * custom EAT claim with a value consisting of a CBOR byte string. The size of
 * this string will normally be 32 bytes to accommodate a 256 bit hash.
 *
 * \param[in/out] size  As an input value it indicates the size of the caller
 *                      allocated buffer (in bytes) to store the implementation
 *                      ID. At return its value is updated with the exact size
 *                      of the implementation ID.
 * \param[out]    buf   Pointer to the buffer to store the implementation ID
 *
 * \return  Returns error code specified in \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_plat_get_implementation_id(uint32_t *size,
                                                   uint8_t  *buf);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_DEVICE_ID_H__ */
