/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_EC_WEIERSTRASS_H__
#define __CC3XX_EC_WEIERSTRASS_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_pka.h"
#include "cc3xx_ec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                        Validate an affine point.
 *
 * @param[in]  curve             A pointer to an initialized weierstrass curve
 *                               object
 * @param[in]  p                 A pointer to the affine point to validate.
 *
 * @return                       true if the affine point a valid point on
 *                               the curve, false if it isn't.
 */
bool cc3xx_lowlevel_ec_weierstrass_validate_point(cc3xx_ec_curve_t *curve,
                                                  cc3xx_ec_point_affine *p);

/**
 * @brief                        Add two affine points
 *
 * @param[in]  curve             A pointer to an initialized weierstrass curve
 *                               object
 * @param[in]  p                 A pointer to the first affine point object to
 *                               add.
 * @param[in]  q                 A pointer to the secnd affine point object to
 *                               add.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_weierstrass_add_points(cc3xx_ec_curve_t *curve,
                                                     cc3xx_ec_point_affine *p,
                                                     cc3xx_ec_point_affine *q,
                                                     cc3xx_ec_point_affine *res);

/**
 * @brief                        Double an affine point
 *
 * @param[in]  curve             A pointer to an initialized weierstrass curve
 *                               object.
 * @param[in]  p                 A pointer to the affine point object to double.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_weierstrass_double_point(cc3xx_ec_curve_t *curve,
                                                       cc3xx_ec_point_affine *p,
                                                       cc3xx_ec_point_affine *res);

/**
 * @brief                        Negate an affine point
 *
 * @param[in]  p                 A pointer to the affine point object to negate.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 */
void cc3xx_lowlevel_ec_weierstrass_negate_point(cc3xx_ec_point_affine *p,
                                                cc3xx_ec_point_affine *res);

/**
 * @brief                        Multiply an affine point by a scalar value
 *
 * @note                         This function is side-channel protected and
 *                               may be used on secret values. By default it is
 *                               protected against timing attacks, further
 *                               mitigations can be enabled by config.
 *
 * @param[in]  curve             A pointer to an initialized weierstrass curve
 *                               object.
 * @param[in]  p                 A pointer to the affine point object to
 *                               multiply.
 * @param[in]  scalar            The scalar value to multiply the point by.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_weierstrass_multipy_point_by_scalar(
                                             cc3xx_ec_curve_t *curve,
                                             cc3xx_ec_point_affine *p,
                                             cc3xx_pka_reg_id_t scalar,
                                             cc3xx_ec_point_affine *res);

/**
 * @brief                        Multiply two scalar by two separate affine
 *                               values, and then add the points. This function
 *                               may use the Shamir trick, if it is enabled by
 *                               config.
 *
 * @note                         This function must _not_ be used on secret
 *                               values, in case the Shamir trick is used which
 *                               is not side-channel protected.
 *
 * @param[in]  curve             A pointer to an initialized weierstrass curve
 *                               object
 * @param[in]  p1                A pointer to the first affine point object to
 *                               multiply.
 * @param[in]  scalar1           The scalar value to multiply the first point
 *                               by.
 * @param[in]  p2                A pointer to the second affine point object to
 *                               multiply.
 * @param[in]  scalar2           The scalar value to multiply the second point
 *                               by.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_weierstrass_shamir_multiply_points_by_scalars_and_add(
                                             cc3xx_ec_curve_t *curve,
                                             cc3xx_ec_point_affine *p1,
                                             cc3xx_pka_reg_id_t    scalar1,
                                             cc3xx_ec_point_affine *p2,
                                             cc3xx_pka_reg_id_t    scalar2,
                                             cc3xx_ec_point_affine *res);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_EC_WEIERSTRASS_H__ */
