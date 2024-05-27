/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_EC_PROJECTIVE_POINT_H__
#define __CC3XX_EC_PROJECTIVE_POINT_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_ec.h"

typedef struct {
    cc3xx_pka_reg_id_t x;
    cc3xx_pka_reg_id_t y;
    cc3xx_pka_reg_id_t z;
} cc3xx_ec_point_projective;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                        Allocate a projective EC point.
 *
 * @return                       An initialized projective EC point.
 */
cc3xx_ec_point_projective cc3xx_lowlevel_ec_allocate_projective_point(void);

/**
 * @brief                        Free a projective EC point.
 *
 * @param[in]  p                 A pointer to the projective point to free.
 */
void cc3xx_lowlevel_ec_free_projective_point(cc3xx_ec_point_projective *p);

/**
 * @brief                        Copy a projective EC point.
 *
 * @param[in]  p                 A pointer to the projective point to copy.
 * @param[out] res               A pointer to the projective point to copy into.
 */
void cc3xx_lowlevel_ec_copy_projective_point(cc3xx_ec_point_projective *p,
                                             cc3xx_ec_point_projective *res);

/**
 * @brief                        Test if a projective point is infinity.
 *
 * @param[in]  p                 A pointer to the projective point to test.
 *
 * @return                       true if the projective point is the infinity
 *                               point, false if it isn't.
 */
bool cc3xx_lowlevel_ec_projective_point_is_infinity(cc3xx_ec_point_projective *p);

/**
 * @brief                        Convert an affine point to a Jacobian-form
 *                               projective point. Xa = Xj / Zj^2,
 *                               Ya = Yj / Zj^2. Should not be used on secret
 *                               data.
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the projective point to copy.
 * @param[out] res               A pointer to the projective point to copy into.
 */
void cc3xx_lowlevel_ec_affine_to_jacobian(cc3xx_ec_curve_t *curve,
                                          cc3xx_ec_point_affine *p,
                                          cc3xx_ec_point_projective *res);

/**
 * @brief                        Convert an affine point to a Jacobian-form
 *                               projective point, with a random Z coordinate.
 *                               Xa = Xj / Zj^2, Ya = Yj / Zj^2.
 *
 * @note                         If DPA mitigations are disabled by config, this
 *                               function falls back to having a Z coordinate of
 *                               1.
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the affine point to convert.
 * @param[out] res               A pointer to the Jacobian point object.
 */
void cc3xx_lowlevel_ec_affine_to_jacobian_with_random_z(cc3xx_ec_curve_t *curve,
                                                        cc3xx_ec_point_affine *p,
                                                        cc3xx_ec_point_projective *res);

/**
 * @brief                        Convert a Jacobian-form projective point to an
 *                               Affine point.
 *                               Xa = Xj / Zj^2, Ya = Yj / Zj^2.
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the Jacobian point to convert.
 * @param[out] res               A pointer to the affine point object.
 */
cc3xx_err_t cc3xx_lowlevel_ec_jacobian_to_affine(cc3xx_ec_curve_t *curve,
                                                 cc3xx_ec_point_projective *p,
                                                 cc3xx_ec_point_affine *res);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_EC_PROJECTIVE_POINT_H__ */
