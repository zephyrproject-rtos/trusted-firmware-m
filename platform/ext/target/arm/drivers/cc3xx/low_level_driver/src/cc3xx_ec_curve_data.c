/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "cc3xx_ec_curve_data.h"

#ifndef CC3XX_CONFIG_FILE
#include "cc3xx_config.h"
#else
#include CC3XX_CONFIG_FILE
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_192_R1_ENABLE
const cc3xx_ec_curve_data_t secp_192_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 24,
    .field_modulus = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 24,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF},
    .field_param_b = {0xC146B9B1, 0xFEB8DEEC, 0x72243049, 0x0FA7E9AB,
                      0xE59C80E7, 0x64210519},

    .generator_x = {0x82FF1012, 0xF4FF0AFD, 0x43A18800, 0x7CBF20EB,
                    0xB03090F6, 0x188DA80E},
    .generator_y = {0x1E794811, 0x73F977A1, 0x6B24CDD5, 0x631011ED,
                    0xFFC8DA78, 0x07192B95},

    .order = {0xB4D22831, 0x146BC9B1, 0x99DEF836, 0xFFFFFFFF,
              0xFFFFFFFF, 0xFFFFFFFF},

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_224_R1_ENABLE
const cc3xx_ec_curve_data_t secp_224_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 28,
    .field_modulus = {0x00000001, 0x00000000, 0x00000000, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 28,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .field_param_b = {0x2355FFB4, 0x270B3943, 0xD7BFD8BA, 0x5044B0B7,
                      0xF5413256, 0x0C04B3AB, 0xB4050A85},

    .generator_x = {0x115C1D21, 0x343280D6, 0x56C21122, 0x4A03C1D3,
                    0x321390B9, 0x6BB4BF7F, 0xB70E0CBD},
    .generator_y = {0x85007E34, 0x44D58199, 0x5A074764, 0xCD4375A0,
                    0x4C22DFE6, 0xB5F723FB, 0xBD376388},

    .order = {0x5C5C2A3D, 0x13DD2945, 0xE0B8F03E, 0xFFFF16A2,
              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},

    .cofactor = 1,
    .recommended_bits_for_generation = 224,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_256_R1_ENABLE
const cc3xx_ec_curve_data_t secp_256_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 32,
    .field_modulus = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
                      0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF},
    .modulus_size = 32,

    .barrett_tag = {0xFFFFFFFF, 0x0000007F, 0x00000080, 0x00000000,
                    0x00000000, 0xFFFFFFFF, 0x0000007F, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
                      0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF},
    .field_param_b = {0x27D2604B, 0x3BCE3C3E, 0xCC53B0F6, 0x651D06B0,
                      0x769886BC, 0xB3EBBD55, 0xAA3A93E7, 0x5AC635D8},

    .generator_x = {0xD898C296, 0xF4A13945, 0x2DEB33A0, 0x77037D81,
                    0x63A440F2, 0xF8BCE6E5, 0xE12C4247, 0x6B17D1F2},
    .generator_y = {0x37BF51F5, 0xCBB64068, 0x6B315ECE, 0x2BCE3357,
                    0x7C0F9E16, 0x8EE7EB4A, 0xFE1A7F9B, 0x4FE342E2},

    .order = {0xFC632551, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
              0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF},

    .cofactor = 1,
    .recommended_bits_for_generation = 352,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_384_R1_ENABLE
const cc3xx_ec_curve_data_t secp_384_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 48,
    .field_modulus = {0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF,
                      0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 48,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0xFFFFFFFC, 0x00000000, 0x00000000, 0xFFFFFFFF,
                      0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .field_param_b = {0xD3EC2AEF, 0x2A85C8ED, 0x8A2ED19D, 0xC656398D,
                      0x5013875A, 0x0314088F, 0xFE814112, 0x181D9C6E,
                      0xE3F82D19, 0x988E056B, 0xE23EE7E4, 0xB3312FA7},

    .generator_x = {0x72760AB7, 0x3A545E38, 0xBF55296C, 0x5502F25D,
                    0x82542A38, 0x59F741E0, 0x8BA79B98, 0x6E1D3B62,
                    0xF320AD74, 0x8EB1C71E, 0xBE8B0537, 0xAA87CA22},
    .generator_y = {0x90EA0E5F, 0x7A431D7C, 0x1D7E819D, 0x0A60B1CE,
                    0xB5F0B8C0, 0xE9DA3113, 0x289A147C, 0xF8F41DBD,
                    0x9292DC29, 0x5D9E98BF, 0x96262C6F, 0x3617DE4A},

    .order = {0xCCC52973, 0xECEC196A, 0x48B0A77A, 0x581A0DB2,
              0xF4372DDF, 0xC7634D81, 0xFFFFFFFF, 0xFFFFFFFF,
              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},

    .cofactor = 1,
    .recommended_bits_for_generation = 384,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_521_R1_ENABLE
const cc3xx_ec_curve_data_t secp_521_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 68,
    .field_modulus = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0x000001FF},
    .modulus_size = 68,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                      0x000001FF},
    .field_param_b = {0x6B503F00, 0xEF451FD4, 0x3D2C34F1, 0x3573DF88,
                      0x3BB1BF07, 0x1652C0BD, 0xEC7E937B, 0x56193951,
                      0x8EF109E1, 0xB8B48991, 0x99B315F3, 0xA2DA725B,
                      0xB68540EE, 0x929A21A0, 0x8E1C9A1F, 0x953EB961,
                      0x00000051},

    .generator_x = {0xC2E5BD66, 0xF97E7E31, 0x856A429B, 0x3348B3C1,
                    0xA2FFA8DE, 0xFE1DC127, 0xEFE75928, 0xA14B5E77,
                    0x6B4D3DBA, 0xF828AF60, 0x053FB521, 0x9C648139,
                    0x2395B442, 0x9E3ECB66, 0x0404E9CD, 0x858E06B7,
                    0x000000C6},
    .generator_y = {0x9FD16650, 0x88BE9476, 0xA272C240, 0x353C7086,
                    0x3FAD0761, 0xC550B901, 0x5EF42640, 0x97EE7299,
                    0x273E662C, 0x17AFBD17, 0x579B4468, 0x98F54449,
                    0x2C7D1BD9, 0x5C8A5FB4, 0x9A3BC004, 0x39296A78,
                    0x00000118},

    .order = {0x91386409, 0xBB6FB71E, 0x899C47AE, 0x3BB5C9B8,
              0xF709A5D0, 0x7FCC0148, 0xBF2F966B, 0x51868783,
              0xFFFFFFFA, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
              0x000001FF},

    .cofactor = 1,
    .recommended_bits_for_generation = 521,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_192_K1_ENABLE
const cc3xx_ec_curve_data_t secp_192_k1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 24,
    .field_modulus = {0xFFFFEE37, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 24,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},
    .field_param_b = {0x00000003, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},

    .generator_x = {0xEAE06C7D, 0x1DA5D1B1, 0x80B7F434, 0x26B07D02,
                    0xC057E9AE, 0xDB4FF10E},
    .generator_y = {0xD95E2F9D, 0x4082AA88, 0x15BE8634, 0x844163D0,
                    0x9C5628A7, 0x9B2F2F6D},

    .order = {0x74DEFD8D, 0x0F69466A, 0x26F2FC17, 0xFFFFFFFE,
              0xFFFFFFFF, 0xFFFFFFFF},

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_224_K1_ENABLE
const cc3xx_ec_curve_data_t secp_224_k1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 32,
    .field_modulus = {0xFFFFE56D, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 28,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x000000FF,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},
    .field_param_b = {0x00000005, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},

    .generator_x = {0xB6B7A45C, 0x0F7E650E, 0xE47075A9, 0x69A467E9,
                    0x30FC28A1, 0x4DF099DF, 0xA1455B33},
    .generator_y = {0x556D61A5, 0xE2CA4BDB, 0xC0B0BD59, 0xF7E319F7,
                    0x82CAFBD6, 0x7FBA3442, 0x7E089FED},

    .order = {0x769FB1F7, 0xCAF0A971, 0xD2EC6184, 0x0001DCE8,
              0x00000000, 0x00000000, 0x00000000, 0x00000001},

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_SECP_256_K1_ENABLE
const cc3xx_ec_curve_data_t secp_256_k1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 32,
    .field_modulus = {0xFFFFFC2F, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    .modulus_size = 32,

    .barrett_tag = {0x00000000, 0x00000000, 0x00000080, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000080,
                    0x00000000, 0x00000000},
    .barrett_tag_size = 40,

    .field_param_a = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},
    .field_param_b = {0x00000007, 0x00000000, 0x00000000, 0x00000000,
                      0x00000000, 0x00000000},

    .generator_x = {0x16F81798, 0x59F2815B, 0x2DCE28D9, 0x029BFCDB,
                    0xCE870B07, 0x55A06295, 0xF9DCBBAC, 0x79BE667E},
    .generator_y = {0xFB10D4B8, 0x9C47D08F, 0xA6855419, 0xFD17B448,
                    0x0E1108A8, 0x5DA4FBFC, 0x26A3C465, 0x483ADA77},

    .order = {0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6,
              0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_192_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_192_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 24,
    .field_modulus = {0xe1a86297, 0x8fce476d, 0x93d18db7, 0xa7a34630,
                      0x932a36cd, 0xc302f41d, },
    .modulus_size = 24,

    .barrett_tag = {0x8b946433, 0x07f51cff, 0x000000a8, },
    .barrett_tag_size = 12,

    .field_param_a = {0xc69a28ef, 0xcae040e5, 0xfe8685c1, 0x9c39c031,
                      0x76b1e0e1, 0x6a911740, },
    .field_param_b = {0x6fbf25c9, 0xca7ef414, 0x4f4496bc, 0xdc721d04,
                      0x7c28cca3, 0x469a28ef, },

    .generator_x = {0x53375fd6, 0x0a2f5c48, 0x6cb0f090, 0x53b033c5,
                    0xaab6a487, 0xc0a0647e, },
    .generator_y = {0xfa299b8f, 0xe6773fa2, 0xc1490002, 0x8b5f4828,
                    0x6abd5bb8, 0x14b69086, },

    .order = {0x9ac4acc1, 0x5be8f102, 0x9e9e916b, 0xa7a3462f,
              0x932a36cd, 0xc302f41d, },

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_224_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_224_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 28,
    .field_modulus = {0x7ec8c0ff, 0x97da89f5, 0xb09f0757, 0x75d1d787,
                      0x2a183025, 0x26436686, 0xd7c134aa, },
    .modulus_size = 28,

    .barrett_tag = {0xe9114ca7, 0xe04cfbc7, 0x00000097, },
    .barrett_tag_size = 12,

    .field_param_a = {0xcad29f43, 0xb0042a59, 0x4e182ad8, 0xc1530b51,
                      0x299803a6, 0xa9ce6c1c, 0x68a5e62c, },
    .field_param_b = {0x386c400b, 0x66dbb372, 0x3e2135d2, 0xa92369e3,
                      0x870713b1, 0xcfe44138, 0x2580f63c, },

    .generator_x = {0xee12c07d, 0x4c1e6efd, 0x9e4ce317, 0xa87dc68c,
                    0x340823b2, 0x2c7e5cf4, 0x0d9029ad, },
    .generator_y = {0x761402cd, 0xcaa3f6d3, 0x354b9e99, 0x4ecdac24,
                    0x24c6b89e, 0x72c0726f, 0x58aa56f7, },

    .order = {0xa5a7939f, 0x6ddebca3, 0xd116bc4b, 0x75d0fb98,
              0x2a183025, 0x26436686, 0xd7c134aa, },

    .cofactor = 1,
    .recommended_bits_for_generation = 224,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_256_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_256_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 32,
    .field_modulus = {0x1f6e5377, 0x2013481d, 0xd5262028, 0x6e3bf623,
                      0x9d838d72, 0x3e660a90, 0xa1eea9bc, 0xa9fb57db, },
    .modulus_size = 32,

    .barrett_tag = {0xe2adbf5d, 0xc60898d0, 0x000000c0, },
    .barrett_tag_size = 12,

    .field_param_a = {0xf330b5d9, 0xe94a4b44, 0x26dc5c6c, 0xfb8055c1,
                      0x417affe7, 0xeef67530, 0xfc2c3057, 0x7d5a0975, },
    .field_param_b = {0xff8c07b6, 0x6bccdc18, 0x5cf7e1ce, 0x95841629,
                      0xbbd77cbf, 0xf330b5d9, 0xe94a4b44, 0x26dc5c6c, },

    .generator_x = {0x9ace3262, 0x3a4453bd, 0xe3bd23c2, 0xb9de27e1,
                    0xfc81b7af, 0x2c4b482f, 0xcb7e57cb, 0x8bd2aeb9, },
    .generator_y = {0x2f046997, 0x5c1d54c7, 0x2ded8e54, 0xc2774513,
                    0x14611dc9, 0x97f8461a, 0xc3dac4fd, 0x547ef835, },

    .order = {0x974856a7, 0x901e0e82, 0xb561a6f7, 0x8c397aa3,
              0x9d838d71, 0x3e660a90, 0xa1eea9bc, 0xa9fb57db, },

    .cofactor = 1,
    .recommended_bits_for_generation = 352,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_320_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_320_r1 = {
    .register_size = 40,
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .field_modulus = {0xf1b32e27, 0xfcd412b1, 0x7893ec28, 0x4f92b9ec,
                      0xf6f40def, 0xf98fcfa6, 0xd201e065, 0xe13c785e,
                      0x36bc4fb7, 0xd35e4720, },
    .modulus_size = 40,

    .barrett_tag = {0x8d509020, 0x072ad2d7, 0x0000009b, },
    .barrett_tag_size = 12,

    .field_param_a = {0x7d860eb4, 0x92f375a9, 0x85ffa9f4, 0x66190eb0,
                      0xf5eb79da, 0xa2a73513, 0x6d3f3bb8, 0x83ccebd4,
                      0x8fbab0f8, 0x3ee30b56, },
    .field_param_b = {0x8fb1f1a6, 0x6f5eb4ac, 0x88453981, 0xcc31dccd,
                      0x9554b49a, 0xe13f4134, 0x40688a6f, 0xd3ad1986,
                      0x9dfdbc42, 0x52088394, },

    .generator_x = {0x39e20611, 0x10af8d0d, 0x10a599c7, 0xe7871e2a,
                    0x0a087eb6, 0xf20137d1, 0x8ee5bfe6, 0x5289bcc4,
                    0xfb53d8b8, 0x43bd7e9a, },
    .generator_y = {0x692e8ee1, 0xd35245d1, 0xaaac6ac7, 0xa9c77877,
                    0x117182ea, 0x0743ffed, 0x7f77275e, 0xab409324,
                    0x45ec1cc8, 0x14fdd055, },

    .order = {0x44c59311, 0x8691555b, 0xee8658e9, 0x2d482ec7,
              0xb68f12a3, 0xf98fcfa5, 0xd201e065, 0xe13c785e,
              0x36bc4fb7, 0xd35e4720, },

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_384_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_384_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 48,
    .field_modulus = {0x3107ec53, 0x87470013, 0x901d1a71, 0xacd3a729,
                      0x7fb71123, 0x12b1da19, 0xed5456b4, 0x152f7109,
                      0x50e641df, 0x0f5d6f7e, 0xa3386d28, 0x8cb91e82, },
    .modulus_size = 48,

    .barrett_tag = {0xc7635c7f, 0xdabad8b6, 0x000000e8, },
    .barrett_tag_size = 12,

    .field_param_a = {0x22ce2826, 0x04a8c7dd, 0x503ad4eb, 0x8aa5814a,
                      0xba91f90f, 0x139165ef, 0x4fb22787, 0xc2bea28e,
                      0xce05afa0, 0x3c72080a, 0x3d8c150c, 0x7bc382c6, },
    .field_param_b = {0xfa504c11, 0x3ab78696, 0x95dbc994, 0x7cb43902,
                      0x3eeb62d5, 0x2e880ea5, 0x07dcd2a6, 0x2fb77de1,
                      0x16f0447c, 0x8b39b554, 0x22ce2826, 0x04a8c7dd, },

    .generator_x = {0x47d4af1e, 0xef87b2e2, 0x36d646aa, 0xe826e034,
                    0x0cbd10e8, 0xdb7fcafe, 0x7ef14fe3, 0x8847a3e7,
                    0xb7c13f6b, 0xa2a63a81, 0x68cf45ff, 0x1d1c64f0, },
    .generator_y = {0x263c5315, 0x42820341, 0x77918111, 0x0e464621,
                    0xf9912928, 0xe19c054f, 0xfeec5864, 0x62b70b29,
                    0x95cfd552, 0x5cb1eb8e, 0x20f9c2a4, 0x8abe1d75, },

    .order = {0xe9046565, 0x3b883202, 0x6b7fc310, 0xcf3ab6af,
              0xac0425a7, 0x1f166e6c, 0xed5456b3, 0x152f7109,
              0x50e641df, 0x0f5d6f7e, 0xa3386d28, 0x8cb91e82, },

    .cofactor = 1,
    .recommended_bits_for_generation = 384,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_BRAINPOOLP_512_R1_ENABLE
const cc3xx_ec_curve_data_t brainpoolp_512_r1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 64,
    .field_modulus = {0x583a48f3, 0x28aa6056, 0x2d82c685, 0x2881ff2f,
                      0xe6a380e6, 0xaecda12a, 0x9bc66842, 0x7d4d9b00,
                      0x70330871, 0xd6639cca, 0xb3c9d20e, 0xcb308db3,
                      0x33c9fc07, 0x3fd4e6ae, 0xdbe9c48b, 0xaadd9db8, },
    .modulus_size = 64,

    .barrett_tag = {0x6d75c50a, 0xc6bfa76b, 0x000000bf, },
    .barrett_tag_size = 12,

    .field_param_a = {0x77fc94ca, 0xe7c1ac4d, 0x2bf2c7b9, 0x7f1117a7,
                      0x8b9ac8b5, 0x0a2ef1c9, 0xa8253aa1, 0x2ded5d5a,
                      0xea9863bc, 0xa83441ca, 0x3df91610, 0x94cbdd8d,
                      0xac234cc5, 0xe2327145, 0x8b603b89, 0x7830a331, },
    .field_param_b = {0x8016f723, 0x2809bd63, 0x5ebae5dd, 0x984050b7,
                      0xdc083e67, 0x77fc94ca, 0xe7c1ac4d, 0x2bf2c7b9,
                      0x7f1117a7, 0x8b9ac8b5, 0x0a2ef1c9, 0xa8253aa1,
                      0x2ded5d5a, 0xea9863bc, 0xa83441ca, 0x3df91610, },

    .generator_x = {0xbcb9f822, 0x8b352209, 0x406a5e68, 0x7c6d5047,
                    0x93b97d5f, 0x50d1687b, 0xe2d0d48d, 0xff3b1f78,
                    0xf4d0098e, 0xb43b62ee, 0xb5d916c1, 0x85ed9f70,
                    0x9c4c6a93, 0x5a21322e, 0xd82ed964, 0x81aee4bd, },
    .generator_y = {0x3ad80892, 0x78cd1e0f, 0xa8f05406, 0xd1ca2b2f,
                    0x8a2763ae, 0x5bca4bd8, 0x4a5f485e, 0xb2dcde49,
                    0x881f8111, 0xa000c55b, 0x24a57b1a, 0xf209f700,
                    0xcf7822fd, 0xc0eabfa9, 0x566332ec, 0x7dde385d, },

    .order = {0x9ca90069, 0xb5879682, 0x085ddadd, 0x1db1d381,
              0x7fac1047, 0x41866119, 0x4ca92619, 0x553e5c41,
              0x70330870, 0xd6639cca, 0xb3c9d20e, 0xcb308db3,
              0x33c9fc07, 0x3fd4e6ae, 0xdbe9c48b, 0xaadd9db8, },

    .cofactor = 1,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_FRP_256_V1_ENABLE
const cc3xx_ec_curve_data_t frp_256_v1 = {
    .type = CC3XX_EC_CURVE_TYPE_WEIERSTRASS,
    .register_size = 32,
    .field_modulus = {0xd86e9c03, 0xe8fcf353, 0xabc8ca6d, 0x3961adbc,
                      0xce42435b, 0x10126de8, 0x0b3ad58f, 0xf1fd178c, },
    .modulus_size = 32,

    .barrett_tag = {0xe9582079, 0x694bee63, 0x00000087, },
    .barrett_tag_size = 12,

    .field_param_a = {0xd86e9c00, 0xe8fcf353, 0xabc8ca6d, 0x3961adbc,
                      0xce42435b, 0x10126de8, 0x0b3ad58f, 0xf1fd178c, },
    .field_param_b = {0x7b7bb73f, 0x3075ed96, 0xe4b1a180, 0xdfec0c9a,
                      0x4a44c00f, 0x0d4aba75, 0x5428a930, 0xee353fca, },

    .generator_x = {0xd98f5cff, 0x64c97a2d, 0xaf98b701, 0x8c27d2dc,
                    0x49d42395, 0x31183d47, 0x56c139eb, 0xb6b3d4c3, },
    .generator_y = {0x54062cfb, 0x83115a15, 0xe8e4c9e1, 0x2701c307,
                    0xf3ecef8c, 0x1f9271f0, 0xc8b20491, 0x6142e0f7, },

    .order = {0xc6d655e1, 0x1ffdd459, 0x40d2bf94, 0x53dc67e1,
              0xce42435b, 0x10126de8, 0x0b3ad58f, 0xf1fd178c, },

    .cofactor = 1,
    .recommended_bits_for_generation = 352,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_25519_ENABLE
const cc3xx_ec_curve_data_t curve_25519 = {
    .type = CC3XX_EC_CURVE_TYPE_MONTGOMERY,
    .recommended_bits_for_generation = 252,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_448_ENABLE
const cc3xx_ec_curve_data_t curve_448 = {
    .type = CC3XX_EC_CURVE_TYPE_MONTGOMERY,
    .recommended_bits_for_generation = 446,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_ED25519_ENABLE
const cc3xx_ec_curve_data_t ed25519 = {
    .type = CC3XX_EC_CURVE_TYPE_TWISTED_EDWARDS,
    .recommended_bits_for_generation = 252,
};
#endif

#ifdef CC3XX_CONFIG_EC_CURVE_ED448_ENABLE
const cc3xx_ec_curve_data_t ed448 = {
    .type = CC3XX_EC_CURVE_TYPE_TWISTED_EDWARDS,
    .recommended_bits_for_generation = 446,
};
#endif
