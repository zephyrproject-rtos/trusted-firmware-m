/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_ECDH_H__
#define __CC3XX_ECDH_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_error.h"
#include "cc3xx_ec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                         Generate a shared secret following the ECDH algorithm
 *
 * @param[in] curve_id            The ID of the curve to use.
 * @param[in] private_key         The buffer to load the private key from.
 * @param[in] private_key_len     The size of the private key buffer.
 * @param[in] public_key_x        The buffer to load the public key x coord from.
 * @param[in] public_key_x_len    The size of the public key x coord buffer.
 * @param[in] public_key_y        The buffer to load the public key x coord from.
 * @param[in] public_key_y_len    The size of the public key y coord key buffer.
 * @param[out] shared_secret      The buffer to write the shared secret into.
 * @param[in] shared_secret_len   The size of the shared secret key buffer.
 * @param[out] shared_secret_size The size of the shared secret written into
 *                                the \a shared_secret buffer.
 *
 * @return cc3xx_err_t            CC3XX_ERR_SUCCESS on success, another
 *                                cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ecdh(cc3xx_ec_curve_id_t curve_id,
                                const uint32_t *private_key, size_t private_key_len,
                                const uint32_t *public_key_x, size_t public_key_x_len,
                                const uint32_t *public_key_y, size_t public_key_y_len,
                                uint32_t *shared_secret, size_t shared_secret_len,
                                size_t *shared_secret_size);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_ECDH_H__ */
