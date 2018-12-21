/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "platform/include/tfm_plat_defs.h"
#include "platform/include/tfm_plat_crypto_keys.h"

/*
 * This file contains the hard coded version of the ECDSA P-256 key pair in:
 * platform/common/tfm_initial_attestation_key.pem
 *
 * This key is used to sign the initial attestation token.
 * The key pair is stored in raw format, without any encoding(ASN.1, COSE).
 *
 * This ECDSA P-256 key is the construction of:
 *   - private key:      32 bytes
 *   - public key:
 *       - X-coordinate: 32 bytes
 *       - Y-coordinate: 32 bytes
 *
 * The hash of the raw public key (H(X || Y)) is also included, because it is
 * used as an instance ID. It is a unique identifier of the device instance.
 *
 * Instance ID is mapped to:
 *   - UEID in the EAT token
 *
 *   #######  DO NOT USE THIS KEY IN PRODUCTION #######
 */

/* Type of the EC curve which the key belongs to */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const enum ecc_curve_t initial_attestation_curve_type = P_256;

/* Initial attestation private key in raw format, without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MUST present on the device-
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint8_t initial_attestation_private_key[] =
{
    0xC7, 0x46, 0x70, 0xBC, 0xB7, 0xE8, 0x5B, 0x38,
    0x03, 0xEF, 0xB4, 0x28, 0x94, 0x04, 0x92, 0xE7,
    0x3E, 0x3F, 0xE9, 0xD4, 0xF7, 0xB5, 0xA8, 0xAD,
    0x5E, 0x48, 0x0C, 0xBD, 0xBC, 0xB5, 0x54, 0xC2
};

TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint32_t initial_attestation_private_key_size =
        sizeof(initial_attestation_private_key);

/* Initial attestation x-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint8_t initial_attestation_public_x_key[] =
{
    0xDC, 0xF0, 0xD0, 0xF4, 0xBC, 0xD5, 0xE2, 0x6A,
    0x54, 0xEE, 0x36, 0xCA, 0xD6, 0x60, 0xD2, 0x83,
    0xD1, 0x2A, 0xBC, 0x5F, 0x73, 0x07, 0xDE, 0x58,
    0x68, 0x9E, 0x77, 0xCD, 0x60, 0x45, 0x2E, 0x75,
};

TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint32_t initial_attestation_public_x_key_size =
        sizeof(initial_attestation_public_x_key);

/* Initial attestation y-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint8_t initial_attestation_public_y_key[] =
{
    0x8C, 0xBA, 0xDB, 0x5F, 0xE9, 0xF8, 0x9A, 0x71,
    0x07, 0xE5, 0xA2, 0xE8, 0xEA, 0x44, 0xEC, 0x1B,
    0x09, 0xB7, 0xDA, 0x2A, 0x1A, 0x82, 0xA0, 0x25,
    0x2A, 0x4C, 0x1C, 0x26, 0xEE, 0x1E, 0xD7, 0xCF,
};

TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint32_t initial_attestation_public_y_key_size =
        sizeof(initial_attestation_public_y_key);

/* Hash (SHA256) of initial attestation public key */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint8_t initial_attestation_raw_public_key_hash[] =
{
    0xf4, 0x0c, 0x8f, 0xbf, 0x12, 0xdb, 0x78, 0x2a,
    0xfd, 0xf4, 0x75, 0x96, 0x6a, 0x06, 0x82, 0x36,
    0xe0, 0x32, 0xab, 0x80, 0xd1, 0xb7, 0xf1, 0xbc,
    0x9f, 0xe7, 0xd8, 0x7a, 0x88, 0xcb, 0x26, 0xd0,
};

TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_INITIAL_ATTESTATION")
const uint32_t initial_attestation_raw_public_key_hash_size =
        sizeof(initial_attestation_raw_public_key_hash);
