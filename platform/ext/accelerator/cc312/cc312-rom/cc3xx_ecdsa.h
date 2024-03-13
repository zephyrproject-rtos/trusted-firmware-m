/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_ECDSA_H__
#define __CC3XX_ECDSA_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_error.h"
#include "cc3xx_ec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC3XX_ECDSA_MAX_PRIVATE_KEY_SIZE      CC3XX_EC_MAX_POINT_SIZE
#define CC3XX_ECDSA_MAX_PUBLIC_COORD_KEY_SIZE CC3XX_EC_MAX_POINT_SIZE

/**
 * @brief                        Generate an ECDSA private key
 *
 * @param[in]  curve_id          The ID of the curve to use.
 * @param[out] private_key       The buffer to write the private key into.
 * @param[in]  private_key_len   The size of the private key buffer.
 * @param[out] private_key_size  The size of the private key written into the
 *                               buffer.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ecdsa_genkey(cc3xx_ec_curve_id_t curve_id,
                                        uint32_t *private_key,
                                        size_t private_key_len,
                                        size_t *private_key_size);

/**
 * @brief                        Calculate an ECDSA public key from a private key
 *
 * @param[in]  curve_id          The ID of the curve that the private key
 *                               belongs to.
 * @param[in]  private_key       The buffer to load the private key from.
 * @param[in]  private_key_len   The size of the private key buffer.
 * @param[out] public_key_x      The buffer to write the public key x coord into.
 * @param[in]  public_key_x_len  The size of the public key x coord buffer.
 * @param[out] public_key_x_size The size of the public key x coord written into
 *                               the buffer.
 * @param[out] public_key_y      The buffer to write the public key y coord into.
 * @param[in]  public_key_y_len  The size of the public key y coord buffer.
 * @param[out] public_key_y_size The size of the public key y coord written into
 *                               the buffer.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ecdsa_getpub(cc3xx_ec_curve_id_t curve_id,
                                        const uint32_t *private_key, size_t private_key_len,
                                        uint32_t *public_key_x, size_t public_key_x_len,
                                        size_t *public_key_x_size,
                                        uint32_t *public_key_y, size_t public_key_y_len,
                                        size_t *public_key_y_size);

/**
 * @brief                        Generate an ECDSA signature
 *
 * @param[in]  curve_id          The ID of the curve that the private key
 *                               belongs to.
 * @param[in]  private_key       The buffer to load the private key from.
 * @param[in]  private_key_len   The size of the private key buffer.
 * @param[in]  hash              The buffer to read the hash from.
 * @param[in]  hash_len          The size of the hash buffer.
 * @param[out] sig_r             The buffer to read the signature r param from.
 * @param[in]  sig_r_len         The size of the signature r param buffer.
 * @param[out] sig_r_size        The size of the signature r param written into
 *                               the buffer.
 * @param[out] sig_s             The buffer to read the signature s param from.
 * @param[in]  sig_s_len         The size of the signature s param buffer.
 * @param[out] sig_s_size        The size of the signature s param written into
 *                               the buffer.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ecdsa_sign(cc3xx_ec_curve_id_t curve_id,
                                      const uint32_t *private_key, size_t private_key_len,
                                      const uint32_t *hash, size_t hash_len,
                                      uint32_t *sig_r, size_t sig_r_len, size_t *sig_r_size,
                                      uint32_t *sig_s, size_t sig_s_len, size_t *sig_s_size);
/**
 * @brief                        Verify an ECDSA signature
 *
 * @param[in]  curve_id          The ID of the curve that the signature was
 *                               generated using.
 * @param[in]  public_key_x      The buffer to read the public key x coord from.
 * @param[in]  public_key_x_len  The size of the public key x coord buffer.
 * @param[in]  public_key_y      The buffer to read the public key y coord from.
 * @param[in]  public_key_y_len  The size of the public key y coord buffer.
 * @param[in]  hash              The buffer to read the hash from.
 * @param[in]  hash_len          The size of the hash buffer.
 * @param[in]  sig_r             The buffer to read the signature r param from.
 * @param[in]  sig_r_len         The size of the signature r param buffer.
 * @param[in]  sig_s             The buffer to read the signature s param from.
 * @param[in]  sig_s_len         The size of the signature s param buffer.
 *
 * @return                       CC3XX_ERR_SUCCESS on signature validation
 *                               success, another cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ecdsa_verify(cc3xx_ec_curve_id_t curve_id,
                                        const uint32_t *public_key_x,
                                        size_t public_key_x_len,
                                        const uint32_t *public_key_y,
                                        size_t public_key_y_len,
                                        const uint32_t *hash, size_t hash_len,
                                        const uint32_t *sig_r, size_t sig_r_len,
                                        const uint32_t *sig_s, size_t sig_s_len);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_ECDSA_H__ */
