/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_EC_H__
#define __CC3XX_EC_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_ec_curve_data.h"
#include "cc3xx_error.h"
#include "cc3xx_pka.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A macro to check if the curve_id is invalid. A curve ID is invalid if the
 *        driver does not support it, or if an invalid translation from a PSA type
 *        has been performed
 */
#define CC3XX_IS_CURVE_ID_INVALID(curve_id)                  \
    (((curve_id) >= _CURVE_ID_MAX) ||                        \
     (cc3xx_lowlevel_ec_get_curve_data(curve_id) == NULL))

/**
 * @brief A representation of an EC point on the PKA engine
 *        using affine coordinates
 */
typedef struct {
    cc3xx_pka_reg_id_t x;
    cc3xx_pka_reg_id_t y;
} cc3xx_ec_point_affine;

/**
 * @brief A mapping of the curve parameters on the PKA engine
 *
 */
typedef struct {
    cc3xx_ec_curve_id_t id;
    size_t modulus_size;
    cc3xx_ec_curve_type_t type;
    cc3xx_pka_reg_id_t field_modulus;
    cc3xx_pka_reg_id_t param_a;
    cc3xx_pka_reg_id_t param_b;
    cc3xx_ec_point_affine generator;
    cc3xx_pka_reg_id_t order;
    uint32_t cofactor;
} cc3xx_ec_curve_t;

/**
 * @brief                        Initialize an Elliptic Curve operation
 *
 * @param[in]  curve_id          The ID of the curve to use.
 * @param[out] curve             An initialized curve object to use in future
 *                               operations.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_init(cc3xx_ec_curve_id_t id,
                                   cc3xx_ec_curve_t *curve);

/**
 * @brief                        Allocate an affine point
 *
 * @return                       An initialized affine point object.
 */
cc3xx_ec_point_affine cc3xx_lowlevel_ec_allocate_point(void);

/**
 * @brief                        Allocate an affine point and initialize it with
 *                               a value
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  x                 The buffer to read the x coordinate from.
 * @param[in]  x_len             The size of the x coordinate buffer.
 * @param[in]  y                 The buffer to read the y coordinate from.
 * @param[in]  y_len             The size of the y coordinate buffer.
 * @param[out] res               A pointer to the affine point object to
 *                               initialize with the value.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_allocate_point_from_data(cc3xx_ec_curve_t *curve,
                                                       const uint32_t *x,
                                                       size_t x_len,
                                                       const uint32_t *y,
                                                       size_t y_len,
                                                       cc3xx_ec_point_affine *res);

/**
 * @brief                        Free an affine point
 *
 * @param[in]  p                 The point to free.
 */
void cc3xx_lowlevel_ec_free_point(cc3xx_ec_point_affine *p);

/**
 * @brief                        Add two affine points
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the first affine point object to
 *                               add.
 * @param[in]  q                 A pointer to the second affine point object to
 *                               add.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_add_points(cc3xx_ec_curve_t *curve,
                                         cc3xx_ec_point_affine *p,
                                         cc3xx_ec_point_affine *q,
                                         cc3xx_ec_point_affine *res);

/**
 * @brief                        Double an affine point
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the affine point object to double.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_double_point(cc3xx_ec_curve_t *curve,
                                           cc3xx_ec_point_affine *p,
                                           cc3xx_ec_point_affine *res);

/**
 * @brief                        Multiply an affine point by a scalar value
 *
 * @note                         This function is side-channel protected and
 *                               may be used on secret values. By default it is
 *                               protected against timing attacks, further
 *                               mitigations can be enabled by config.
 *
 * @param[in]  curve             A pointer to an initialized curve object
 * @param[in]  p                 A pointer to the affine point object to
 *                               multiply.
 * @param[in]  scalar            The scalar value to multiply the point by.
 * @param[out] res               A pointer to the affine point object which the
 *                               result will be written to.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_ec_multiply_point_by_scalar(cc3xx_ec_curve_t *curve,
                                                       cc3xx_ec_point_affine *p,
                                                       cc3xx_pka_reg_id_t scalar,
                                                       cc3xx_ec_point_affine *res);

/**
 * @brief                        Multiply two scalars by two separate affine
 *                               values, and then add the points. This function
 *                               may use the Shamir trick, if it is enabled by
 *                               config.
 *
 * @note                         This function must _not_ be used on secret
 *                               values, in case the Shamir trick is used which
 *                               is not side-channel protected.
 *
 * @param[in]  curve             A pointer to an initialized curve object
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
cc3xx_err_t cc3xx_lowlevel_ec_shamir_multiply_points_by_scalars_and_add(
                                             cc3xx_ec_curve_t *curve,
                                             cc3xx_ec_point_affine *p1,
                                             cc3xx_pka_reg_id_t    scalar1,
                                             cc3xx_ec_point_affine *p2,
                                             cc3xx_pka_reg_id_t    scalar2,
                                             cc3xx_ec_point_affine *res);

/**
 * @brief Getter method to return the associated \a modulus_size from a curve.
 *        Note that usually the implementation deals with 4-byte aligned sizes
 *        so for, e.g. 521 bit curves this will be slightly greater than what
 *        would be strictly required
 *
 * @param[in] curve_id An ID of the curve to retrieve the modulus size
 *
 * @return size_t Size in bytes of the modulus
 */
size_t cc3xx_lowlevel_ec_get_modulus_size_from_curve(cc3xx_ec_curve_id_t curve_id);

/**
 * @brief Getter method to retrieve the curve_data of type \a cc3xx_ec_curve_data_t *
 *        from the \a curve_id. Note that the \a curve_id must be in valid range, if
 *        the getter returns NULL it means that support for the curve has not been
 *        compiled in
 *
 * @param[in] curve_id An ID of the curve to retrieve the parameters structure
 *
 * @return cc3xx_ec_curve_data_t* Pointer to the curve data. NULL if support for
 *                                the curve is not compiled in
 */
const cc3xx_ec_curve_data_t *cc3xx_lowlevel_ec_get_curve_data(
                                                    cc3xx_ec_curve_id_t curve_id);

/**
 * @brief De-initializes the Elliptic Curve operation
 *
 */
void cc3xx_lowlevel_ec_uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_EC_H */
