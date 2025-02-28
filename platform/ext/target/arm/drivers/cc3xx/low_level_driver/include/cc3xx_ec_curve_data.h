/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_EC_CURVE_DATA_H__
#define __CC3XX_EC_CURVE_DATA_H__

#include <stdint.h>
#include <stddef.h>

#include "cc3xx_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief List of curve types
 *
 */
typedef enum {
    CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    CC3XX_EC_CURVE_TYPE_MONTGOMERY,
    CC3XX_EC_CURVE_TYPE_TWISTED_EDWARDS,
} cc3xx_ec_curve_type_t;

/**
 * @brief List of curve identifiers
 *
 */
typedef enum {
    CC3XX_EC_CURVE_SECP_192_R1 = 0U,
    CC3XX_EC_CURVE_SECP_224_R1,
    CC3XX_EC_CURVE_SECP_256_R1,
    CC3XX_EC_CURVE_SECP_384_R1,
    CC3XX_EC_CURVE_SECP_521_R1,

    CC3XX_EC_CURVE_SECP_192_K1,
    CC3XX_EC_CURVE_SECP_224_K1,
    CC3XX_EC_CURVE_SECP_256_K1,

    CC3XX_EC_CURVE_BRAINPOOLP_192_R1,
    CC3XX_EC_CURVE_BRAINPOOLP_224_R1,
    CC3XX_EC_CURVE_BRAINPOOLP_256_R1,
    CC3XX_EC_CURVE_BRAINPOOLP_320_R1,
    CC3XX_EC_CURVE_BRAINPOOLP_384_R1,
    CC3XX_EC_CURVE_BRAINPOOLP_512_R1,

    CC3XX_EC_CURVE_FRP_256_V1,

    CC3XX_EC_CURVE_25519,
    CC3XX_EC_CURVE_448,

    CC3XX_EC_CURVE_ED25519,
    CC3XX_EC_CURVE_ED448,
    _CURVE_ID_MAX,
    _CURVE_ID_SIZE_PAD = UINT32_MAX,
} cc3xx_ec_curve_id_t;

#if defined(CC3XX_CONFIG_EC_CURVE_SECP_521_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 68
#elif defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_512_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 64
#elif defined(CC3XX_CONFIG_EC_CURVE_SECP_384_R1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_384_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 48
#elif defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_320_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 40
#elif defined(CC3XX_CONFIG_EC_CURVE_SECP_256_R1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_SECP_256_K1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_256_R1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_FRP_256_V1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 32
#elif defined(CC3XX_CONFIG_EC_CURVE_SECP_224_R1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_SECP_224_K1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_224_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 28
#elif defined(CC3XX_CONFIG_EC_CURVE_SECP_192_R1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_SECP_192_K1_ENABLE) \
  ||  defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_192_R1_ENABLE)
#define CC3XX_EC_MAX_POINT_SIZE 24
#else
#define CC3XX_EC_MAX_POINT_SIZE 0
#endif

#if defined(CC3XX_CONFIG_EC_CURVE_SECP_192_R1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_224_R1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_256_R1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_384_R1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_512_R1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_192_K1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_224_K1_ENABLE) \
 || defined(CC3XX_CONFIG_EC_CURVE_SECP_256_K1_ENABLE)
#define CC3XX_EC_MAX_BARRETT_TAG_SIZE 40
#elif defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_192_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_224_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_256_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_320_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_384_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_512_R1_ENABLE) \
   || defined(CC3XX_CONFIG_EC_CURVE_FRP_256_V1_ENABLE)
#define CC3XX_EC_MAX_BARRETT_TAG_SIZE 12
#else
#define CC3XX_EC_MAX_BARRETT_TAG_SIZE 0
#endif

/**
 * @brief Structure describing Elliptic Curve parameters
 *
 */
typedef struct {
    cc3xx_ec_curve_type_t type;

    size_t modulus_size;
    size_t barrett_tag_size;
    size_t register_size;
    size_t recommended_bits_for_generation; /*!< as per FIPS 186-5 Table A.2 */

    uint32_t field_modulus[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];
    uint32_t barrett_tag[CC3XX_EC_MAX_BARRETT_TAG_SIZE / sizeof(uint32_t)];

    uint32_t field_param_a[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];
    uint32_t field_param_b[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];

    uint32_t generator_x[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];
    uint32_t generator_y[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];

    uint32_t order[CC3XX_EC_MAX_POINT_SIZE / sizeof(uint32_t)];
    uint32_t cofactor;
} cc3xx_ec_curve_data_t;

extern const cc3xx_ec_curve_data_t secp_192_r1;
extern const cc3xx_ec_curve_data_t secp_224_r1;
extern const cc3xx_ec_curve_data_t secp_256_r1;
extern const cc3xx_ec_curve_data_t secp_384_r1;
extern const cc3xx_ec_curve_data_t secp_521_r1;

extern const cc3xx_ec_curve_data_t secp_192_k1;
extern const cc3xx_ec_curve_data_t secp_224_k1;
extern const cc3xx_ec_curve_data_t secp_256_k1;

extern const cc3xx_ec_curve_data_t brainpoolp_192_r1;
extern const cc3xx_ec_curve_data_t brainpoolp_224_r1;
extern const cc3xx_ec_curve_data_t brainpoolp_256_r1;
extern const cc3xx_ec_curve_data_t brainpoolp_320_r1;
extern const cc3xx_ec_curve_data_t brainpoolp_384_r1;
extern const cc3xx_ec_curve_data_t brainpoolp_512_r1;

extern const cc3xx_ec_curve_data_t frp_256_v1;

extern const cc3xx_ec_curve_data_t curve_25519;
extern const cc3xx_ec_curve_data_t curve_448;

extern const cc3xx_ec_curve_data_t ed25519;
extern const cc3xx_ec_curve_data_t ed448;

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_EC_CURVE_DATA_H__ */
