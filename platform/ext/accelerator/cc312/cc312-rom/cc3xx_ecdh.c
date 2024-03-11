/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "cc3xx_ecdh.h"

#include "cc3xx_ec.h"
#include "cc3xx_config.h"

#include <stdint.h>
#include <stddef.h>

#ifdef CC3XX_CONFIG_ECDH_ENABLE
cc3xx_err_t cc3xx_lowlevel_ecdh(cc3xx_ec_curve_id_t curve_id,
                                const uint32_t *private_key, size_t private_key_len,
                                const uint32_t *public_key_x, size_t public_key_x_len,
                                const uint32_t *public_key_y, size_t public_key_y_len,
                                uint32_t *shared_secret, size_t shared_secret_len,
                                size_t *shared_secret_size)
{
    cc3xx_ec_curve_t curve;
    cc3xx_pka_reg_id_t private_key_reg;
    cc3xx_ec_point_affine shared_key_point, public_key_point;
    cc3xx_err_t err;

    /* This sets up various curve parameters into PKA registers */
    err = cc3xx_lowlevel_ec_init(curve_id, &curve);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    if (private_key_len > curve.modulus_size) {
        err = CC3XX_ERR_ECDSA_INVALID_KEY;
        goto out;
    }

    if (public_key_x_len > curve.modulus_size) {
        err = CC3XX_ERR_ECDSA_INVALID_KEY;
        goto out;
    }

    if (public_key_y_len > curve.modulus_size) {
        err = CC3XX_ERR_ECDSA_INVALID_KEY;
        goto out;
    }

    if (shared_secret_len < curve.modulus_size) {
        err = CC3XX_ERR_BUFFER_OVERFLOW;
        goto out;
    }

    shared_key_point = cc3xx_lowlevel_ec_allocate_point();
    public_key_point = cc3xx_lowlevel_ec_allocate_point();
    private_key_reg = cc3xx_lowlevel_pka_allocate_reg();

    cc3xx_lowlevel_pka_write_reg_swap_endian(private_key_reg, private_key, private_key_len);
    cc3xx_lowlevel_pka_write_reg_swap_endian(public_key_point.x, public_key_x, curve.modulus_size);
    cc3xx_lowlevel_pka_write_reg_swap_endian(public_key_point.y, public_key_y, curve.modulus_size);

    err = cc3xx_lowlevel_ec_multipy_point_by_scalar(&curve, &public_key_point, private_key_reg,
                                           &shared_key_point);

    /* SEC1 paragraph 3.3.1: Output the x coordinate only as the shared secret */
    cc3xx_lowlevel_pka_read_reg_swap_endian(shared_key_point.x, shared_secret, curve.modulus_size);
    *shared_secret_size = curve.modulus_size;

out:
    cc3xx_lowlevel_ec_uninit();

    return err;
}
#endif /* CC3XX_CONFIG_ECDH_ENABLE */
