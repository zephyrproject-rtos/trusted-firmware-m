/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "cc3xx_ec_projective_point.h"

#ifndef CC3XX_CONFIG_FILE
#include "cc3xx_config.h"
#else
#include CC3XX_CONFIG_FILE
#endif
#include "cc3xx_pka.h"
#include "cc3xx_ec.h"

#include "fatal_error.h"

cc3xx_ec_point_projective cc3xx_lowlevel_ec_allocate_projective_point(void)
{
    cc3xx_ec_point_projective res;

    res.x = cc3xx_lowlevel_pka_allocate_reg();
    res.y = cc3xx_lowlevel_pka_allocate_reg();
    res.z = cc3xx_lowlevel_pka_allocate_reg();

    return res;
}

void cc3xx_lowlevel_ec_free_projective_point(cc3xx_ec_point_projective *p)
{
    cc3xx_lowlevel_pka_free_reg(p->z);
    cc3xx_lowlevel_pka_free_reg(p->y);
    cc3xx_lowlevel_pka_free_reg(p->x);
}

bool cc3xx_lowlevel_ec_projective_point_is_infinity(cc3xx_ec_point_projective *p)
{
    return cc3xx_lowlevel_pka_are_equal_si(p->z, 0);
}

void cc3xx_lowlevel_ec_copy_projective_point(cc3xx_ec_point_projective *p,
                                    cc3xx_ec_point_projective *res)
{
    cc3xx_lowlevel_pka_copy(p->x, res->x);
    cc3xx_lowlevel_pka_copy(p->y, res->y);
    cc3xx_lowlevel_pka_copy(p->z, res->z);
}

void cc3xx_lowlevel_ec_projective_point_make_infinity(cc3xx_ec_point_projective *res)
{
    cc3xx_lowlevel_pka_clear(res->x);                  /* x = 0 */
    cc3xx_lowlevel_pka_set_to_power_of_two(res->y, 0); /* y = 2^0 = 1 */
    cc3xx_lowlevel_pka_clear(res->z);                  /* z = 0 */
}

void cc3xx_lowlevel_ec_affine_to_jacobian_with_random_z(cc3xx_ec_curve_t *curve,
                                                        cc3xx_ec_point_affine *p,
                                                        cc3xx_ec_point_projective *res)
{
#ifndef CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE
    return cc3xx_lowlevel_ec_affine_to_jacobian(curve, p, res);
#else
    cc3xx_pka_reg_id_t temp = cc3xx_lowlevel_pka_allocate_reg();

    /* Z is random, between 0 and N-1. Realistically, the RNG will never output
     * 0.
     */
    do {
        cc3xx_lowlevel_pka_set_to_random_within_modulus(res->z);
    } while (cc3xx_lowlevel_pka_are_equal_si(res->z, 0));

    /* Construct Z^2 */
    cc3xx_lowlevel_pka_mod_mul(res->z, res->z, temp);

    /* Divide X by Z^2 */
    cc3xx_lowlevel_pka_mod_mul(p->x, temp, res->x);

    /* Construct Z^3 */
    cc3xx_lowlevel_pka_mod_mul(temp, res->z, temp);

    /* Divide Y by Z^3 */
    cc3xx_lowlevel_pka_mod_mul(p->y, temp, res->y);

    cc3xx_lowlevel_pka_free_reg(temp);
#endif /* CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE */
}

void cc3xx_lowlevel_ec_affine_to_jacobian(cc3xx_ec_curve_t *curve,
                                          cc3xx_ec_point_affine *p,
                                          cc3xx_ec_point_projective *res)
{
    cc3xx_lowlevel_pka_copy(p->x, res->x);
    cc3xx_lowlevel_pka_copy(p->y, res->y);
    cc3xx_lowlevel_pka_clear(res->z);
    cc3xx_lowlevel_pka_add_si(res->z, 1, res->z);
}

cc3xx_err_t cc3xx_lowlevel_ec_jacobian_to_affine(cc3xx_ec_curve_t *curve,
                                                 cc3xx_ec_point_projective *p,
                                                 cc3xx_ec_point_affine *res)
{
    if (cc3xx_lowlevel_ec_projective_point_is_infinity(p)) {
        FATAL_ERR(CC3XX_ERR_EC_POINT_IS_INFINITY);
        return CC3XX_ERR_EC_POINT_IS_INFINITY;
    }

    cc3xx_pka_reg_id_t z_inv = cc3xx_lowlevel_pka_allocate_reg();
    cc3xx_pka_reg_id_t temp = cc3xx_lowlevel_pka_allocate_reg();

    /* Construct 1/(Z^2) */
    cc3xx_lowlevel_pka_mod_inv_prime_modulus(p->z, z_inv);
    cc3xx_lowlevel_pka_mod_mul(z_inv, z_inv, temp);

    /* Divide X by Z^2 */
    cc3xx_lowlevel_pka_mod_mul(p->x, temp, res->x);

    /* Construct 1/(Z^3) */
    cc3xx_lowlevel_pka_mod_mul(temp, z_inv, temp);

    /* Divide Y by Z^3 */
    cc3xx_lowlevel_pka_mod_mul(p->y, temp, res->y);

    cc3xx_lowlevel_pka_free_reg(temp);
    cc3xx_lowlevel_pka_free_reg(z_inv);

    return CC3XX_ERR_SUCCESS;
}
