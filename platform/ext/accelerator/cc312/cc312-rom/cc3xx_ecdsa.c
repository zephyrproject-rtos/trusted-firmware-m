/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "cc3xx_ecdsa.h"

#include "cc3xx_ec.h"
#include "cc3xx_config.h"

#include <stdint.h>
#include <stddef.h>

#include "fatal_error.h"

#ifdef CC3XX_CONFIG_ECDSA_KEYGEN_ENABLE
cc3xx_err_t cc3xx_lowlevel_ecdsa_genkey(cc3xx_ec_curve_id_t curve_id,
                                        uint32_t *private_key,
                                        size_t private_key_len,
                                        size_t *private_key_size)
{
    cc3xx_ec_curve_t curve;
    cc3xx_pka_reg_id_t private_key_reg;
    cc3xx_err_t err;

    /* This sets up various curve parameters into PKA registers */
    err = cc3xx_lowlevel_ec_init(curve_id, &curve);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    if (private_key_len < curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_BUFFER_OVERFLOW);
        goto out;
    }

    private_key_reg = cc3xx_lowlevel_pka_allocate_reg();

    do {
        err = cc3xx_lowlevel_pka_set_to_random_within_modulus(private_key_reg);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }
    } while (!cc3xx_lowlevel_pka_greater_than_si(private_key_reg, 0));

    cc3xx_lowlevel_pka_read_reg_swap_endian(private_key_reg, private_key, curve.modulus_size);

    /* Destroy private key register */
    cc3xx_lowlevel_pka_set_to_random(private_key_reg, curve.modulus_size * 8);

    *private_key_size = curve.modulus_size;
out:
    cc3xx_lowlevel_ec_uninit();

    return err;
}

cc3xx_err_t cc3xx_lowlevel_ecdsa_getpub(cc3xx_ec_curve_id_t curve_id,
                                        const uint32_t *private_key, size_t private_key_len,
                                        uint32_t *public_key_x, size_t public_key_x_len,
                                        size_t *public_key_x_size,
                                        uint32_t *public_key_y, size_t public_key_y_len,
                                        size_t *public_key_y_size)
{
    cc3xx_ec_curve_t curve;
    cc3xx_pka_reg_id_t private_key_reg;
    cc3xx_ec_point_affine public_key_point;
    cc3xx_err_t err;

    /* This sets up various curve parameters into PKA registers */
    err = cc3xx_lowlevel_ec_init(curve_id, &curve);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    if (private_key_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }

    if (public_key_x_len < curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_BUFFER_OVERFLOW);
        goto out;
    }

    if (public_key_y_len < curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_BUFFER_OVERFLOW);
        goto out;
    }

    public_key_point = cc3xx_lowlevel_ec_allocate_point();
    private_key_reg = cc3xx_lowlevel_pka_allocate_reg();

    cc3xx_lowlevel_pka_write_reg_swap_endian(private_key_reg, private_key, private_key_len);

    err = cc3xx_lowlevel_ec_multipy_point_by_scalar(&curve, &curve.generator, private_key_reg,
                                           &public_key_point);

    cc3xx_lowlevel_pka_read_reg_swap_endian(public_key_point.x, public_key_x, curve.modulus_size);
    cc3xx_lowlevel_pka_read_reg_swap_endian(public_key_point.y, public_key_y, curve.modulus_size);

    *public_key_x_size = curve.modulus_size;
    *public_key_y_size = curve.modulus_size;

out:
    cc3xx_lowlevel_ec_uninit();

    return err;
}
#endif /* CC3XX_CONFIG_ECDSA_KEYGEN_ENABLE */

#ifdef CC3XX_CONFIG_ECDSA_VERIFY_ENABLE
/* As per NIST FIPS 186-5 section 6.4.2.
 */
cc3xx_err_t cc3xx_lowlevel_ecdsa_verify(cc3xx_ec_curve_id_t curve_id,
                                        const uint32_t *public_key_x,
                                        size_t public_key_x_len,
                                        const uint32_t *public_key_y,
                                        size_t public_key_y_len,
                                        const uint32_t *hash, size_t hash_len,
                                        const uint32_t *sig_r, size_t sig_r_len,
                                        const uint32_t *sig_s, size_t sig_s_len)
{
    cc3xx_ec_curve_t curve;
    cc3xx_ec_point_affine public_key_point;
    cc3xx_ec_point_affine calculated_r_point;
    cc3xx_pka_reg_id_t sig_s_reg;
    cc3xx_pka_reg_id_t sig_r_reg;
    cc3xx_pka_reg_id_t u_reg;
    cc3xx_pka_reg_id_t v_reg;
    cc3xx_pka_reg_id_t hash_reg;
    cc3xx_err_t err;
    struct cc3xx_pka_state_t pka_state;

    /* This sets up various curve parameters into PKA registers */
    err = cc3xx_lowlevel_ec_init(curve_id, &curve);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    if (public_key_x_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }

    if (public_key_y_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }

    if (sig_r_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    if (sig_s_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    /* The hash _can_ be larger than the modulus, but must still fit into the
     * PKA reg.
     */
    cc3xx_lowlevel_pka_get_state(&pka_state, 0, NULL, NULL, NULL);
    if (hash_len > pka_state.reg_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_HASH);
        goto out;
    }

    /* Now that PKA is initialized, allocate the registers / points we'll use */
    sig_r_reg = cc3xx_lowlevel_pka_allocate_reg();
    sig_s_reg = cc3xx_lowlevel_pka_allocate_reg();
    u_reg = cc3xx_lowlevel_pka_allocate_reg();
    v_reg = cc3xx_lowlevel_pka_allocate_reg();
    hash_reg = cc3xx_lowlevel_pka_allocate_reg();

    calculated_r_point = cc3xx_lowlevel_ec_allocate_point();


    /* This validates that the public key point lies on the curve, is not the
     * identity element, and that it multiplied by the group order is infinity.
     */
    err = cc3xx_lowlevel_ec_allocate_point_from_data(&curve,
                                            public_key_x, public_key_x_len,
                                            public_key_y, public_key_y_len,
                                            &public_key_point);
    if (err != CC3XX_ERR_SUCCESS) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    cc3xx_lowlevel_pka_write_reg_swap_endian(sig_s_reg, sig_s, sig_s_len);
    /* Check that s is less than N */
    if (!cc3xx_lowlevel_pka_less_than(sig_s_reg, CC3XX_PKA_REG_N)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }
    /* Check that s is not 0 */
    if (cc3xx_lowlevel_pka_are_equal_si(sig_s_reg, 0)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    /* Check that r is less than N */
    cc3xx_lowlevel_pka_write_reg_swap_endian(sig_r_reg, sig_r, sig_r_len);
    if (!cc3xx_lowlevel_pka_less_than(sig_r_reg, CC3XX_PKA_REG_N)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }
    /* Check that r is not 0 */
    if (cc3xx_lowlevel_pka_are_equal_si(sig_r_reg, 0)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    cc3xx_lowlevel_pka_write_reg_swap_endian(hash_reg, hash, hash_len);
    /* Check that hash is not 0 */
    if (cc3xx_lowlevel_pka_are_equal_si(hash_reg, 0)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }
    /* If the size of the hash is greater than the group order, reduce the amount of
     * bits used by shifting.
     */
    if (hash_len > curve.modulus_size) {
        cc3xx_lowlevel_pka_shift_right_fill_0_ui(hash_reg,
                                                 (hash_len - curve.modulus_size) * 8,
                                                 hash_reg);
    }
    /* Then finally a reduction so we can be sure the hash is below N */
    cc3xx_lowlevel_pka_reduce(hash_reg);

    /* u = hash * s^-1. Put s^-1 into v_reg so we can reuse it for calculating v
     * later
     */
    cc3xx_lowlevel_pka_mod_inv(sig_s_reg, v_reg);
    cc3xx_lowlevel_pka_mod_mul(v_reg, hash_reg, u_reg);

    /* v = r * s^-1 */
    cc3xx_lowlevel_pka_mod_mul(v_reg, sig_r_reg, v_reg);

    /* R1 = [u]G + [v]Q */
    err = cc3xx_lowlevel_ec_shamir_multiply_points_by_scalars_and_add(&curve,
                                                      &curve.generator, u_reg,
                                                      &public_key_point, v_reg,
                                                      &calculated_r_point);
    if (err != CC3XX_ERR_SUCCESS) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
        goto out;
    }

    /* Accept signature if r == r1 mod N */
    cc3xx_lowlevel_pka_reduce(calculated_r_point.x);

    if (cc3xx_lowlevel_pka_are_equal(sig_r_reg, calculated_r_point.x)) {
        err = CC3XX_ERR_SUCCESS;
    } else {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_SIGNATURE_INVALID);
    }

out:
    /* This frees all the PKA registers as it will deinit the PKA engine */
    cc3xx_lowlevel_ec_uninit();

    return err;
}
#endif /* CC3XX_CONFIG_ECDSA_VERIFY_ENABLE */

#ifdef CC3XX_CONFIG_ECDSA_SIGN_ENABLE
cc3xx_err_t cc3xx_lowlevel_ecdsa_sign(cc3xx_ec_curve_id_t curve_id,
                                      const uint32_t *private_key, size_t private_key_len,
                                      const uint32_t *hash, size_t hash_len,
                                      uint32_t *sig_r, size_t sig_r_len, size_t *sig_r_size,
                                      uint32_t *sig_s, size_t sig_s_len, size_t *sig_s_size)
{
    cc3xx_ec_curve_t curve;
    cc3xx_ec_point_affine temp_point;
    cc3xx_pka_reg_id_t sig_s_reg;
    cc3xx_pka_reg_id_t sig_r_reg;
    cc3xx_pka_reg_id_t k_reg;
    cc3xx_pka_reg_id_t temp_reg;
    cc3xx_pka_reg_id_t hash_reg;
    cc3xx_pka_reg_id_t private_key_reg;
    cc3xx_err_t err;

    /* This sets up various curve parameters into PKA registers */
    err = cc3xx_lowlevel_ec_init(curve_id, &curve);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    if (private_key_len > curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }

    if (sig_r_len < curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_BUFFER_OVERFLOW);
        goto out;
    }

    if (sig_s_len < curve.modulus_size) {
        FATAL_ERR(err = CC3XX_ERR_BUFFER_OVERFLOW);
        goto out;
    }


    /* Now that PKA is initialized, allocate the registers / points we'll use */
    sig_r_reg = cc3xx_lowlevel_pka_allocate_reg();
    sig_s_reg = cc3xx_lowlevel_pka_allocate_reg();
    k_reg = cc3xx_lowlevel_pka_allocate_reg();
    temp_reg = cc3xx_lowlevel_pka_allocate_reg();
    hash_reg = cc3xx_lowlevel_pka_allocate_reg();
    private_key_reg = cc3xx_lowlevel_pka_allocate_reg();

    temp_point = cc3xx_lowlevel_ec_allocate_point();

    cc3xx_lowlevel_pka_write_reg_swap_endian(private_key_reg, private_key, private_key_len);
    /* Check that d is less than N */
    if (!cc3xx_lowlevel_pka_less_than(private_key_reg, CC3XX_PKA_REG_N)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }
    /* Check that d is not 0 */
    if (cc3xx_lowlevel_pka_are_equal_si(private_key_reg, 0)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_KEY);
        goto out;
    }

    cc3xx_lowlevel_pka_write_reg_swap_endian(hash_reg, hash, hash_len);
    /* Check that hash is not 0 */
    if (cc3xx_lowlevel_pka_are_equal_si(hash_reg, 0)) {
        FATAL_ERR(err = CC3XX_ERR_ECDSA_INVALID_HASH);
        goto out;
    }
    /* If the size of the hash is greater than the group order, reduce the amount of
     * bits used by shifting.
     */
    if (hash_len > curve.modulus_size) {
        cc3xx_lowlevel_pka_shift_right_fill_0_ui(hash_reg,
                                                 (hash_len - curve.modulus_size) * 8,
                                                 hash_reg);
    }
    /* Then finally a reduction so we can be sure the hash is below N */
    cc3xx_lowlevel_pka_reduce(hash_reg);

    do {
        err = cc3xx_lowlevel_pka_set_to_random_within_modulus(k_reg);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }

        cc3xx_lowlevel_ec_multipy_point_by_scalar(&curve, &curve.generator, k_reg,
                                         &temp_point);
        cc3xx_lowlevel_pka_copy(temp_point.x, sig_r_reg);
        cc3xx_lowlevel_pka_reduce(sig_r_reg);

        if (!cc3xx_lowlevel_pka_greater_than_si(sig_r_reg, 0)) {
            continue;
        }

        cc3xx_lowlevel_pka_mod_mul(sig_r_reg, private_key_reg, temp_reg);
        cc3xx_lowlevel_pka_mod_add(temp_reg, hash_reg, temp_reg);
        cc3xx_lowlevel_pka_mod_inv(k_reg, k_reg);
        cc3xx_lowlevel_pka_mod_mul(k_reg, temp_reg, sig_s_reg);

        /* Destroy K */
        cc3xx_lowlevel_pka_set_to_random(k_reg, curve.modulus_size * 8);
    } while (!cc3xx_lowlevel_pka_greater_than_si(sig_s_reg, 0));

    cc3xx_lowlevel_pka_read_reg_swap_endian(sig_s_reg, sig_s, curve.modulus_size);
    cc3xx_lowlevel_pka_read_reg_swap_endian(sig_r_reg, sig_r, curve.modulus_size);

    /* Destroy private key register */
    cc3xx_lowlevel_pka_set_to_random(private_key_reg, curve.modulus_size * 8);

    *sig_s_size = curve.modulus_size;
    *sig_r_size = curve.modulus_size;

    err = CC3XX_ERR_SUCCESS;
out:
    /* This frees all the PKA registers as it will deinit the PKA engine */
    cc3xx_lowlevel_ec_uninit();

    return err;
}
#endif /* CC3XX_CONFIG_ECDSA_SIGN_ENABLE */
