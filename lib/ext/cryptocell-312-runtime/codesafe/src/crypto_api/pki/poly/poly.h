/*
 * Copyright (c) 2001-2022, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef POLY_H
#define POLY_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#include "cc_error.h"
#include "mbedtls_cc_poly.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! The POLY block size in 32-bit words */
#define CC_POLY_BLOCK_SIZE_IN_WORDS  4
#define CC_POLY_BLOCK_SIZE_IN_BYTES  (CC_POLY_BLOCK_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

#define POLY_PRIME_SIZE_IN_BITS   130
#define POLY_PRIME_SIZE_IN_WORDS  CALC_FULL_32BIT_WORDS(POLY_PRIME_SIZE_IN_BITS)

/*! The POLY PKA registers size in 32-bit words */
#define CC_POLY_PKA_REG_SIZE_IN_PKA_WORDS  4
#define CC_POLY_PKA_REG_SIZE_IN_WORDS  (CC_POLY_PKA_REG_SIZE_IN_PKA_WORDS * (CALC_FULL_32BIT_WORDS(CC_PKA_WORD_SIZE_IN_BITS)))
#define CC_POLY_PKA_REG_SIZE_IN_BYTES  (CC_POLY_PKA_REG_SIZE_IN_WORDS*CC_32BIT_WORD_SIZE)

/**
 * PKA register contexts. Between multipart calls, the PKA engine needs to save
 * and restore the register context. It's composed of the clamped key pair
 * (r,s) 256 bit long and the value of the accumulator register which is mod P,
 * where P is 2^130-5, which in full words is 160 bit long, 5 32-bit words.
 */
typedef struct PolyPkaContext {
    uint32_t key[8]; /*!< (r,s) concatenated with r already clamped */
    uint32_t acc[5]; /*!< Value of the accumulator modulus P, i.e. [0,2^130-5)*/
} PolyPkaContext_t;

/**
 * State information required to support multipart APIs in AEAD for MAC
 * computation. As Poly1305 operates on CC_POLY_BLOCK_SIZE_IN BYTES of data
 * it needs to cache up to CC_POLY_BLOCK_SIZE_IN_BYTES-1 of the input. But
 * for practical reasons (i.e. working on 4-byte aligned buffers) we store an
 * entire block of 16 bytes that can be processed in one go without additional
 * copies
 */
typedef struct PolyState {
    uint32_t msg_state[CC_POLY_BLOCK_SIZE_IN_WORDS]; /*!< Equals 16 bytes of
                                                      *   data
                                                      */
    uint8_t msg_state_size;  /*!< Size of the message buffered in msg_state */
    PolyPkaContext_t context; /*!< PKA registers context (clamped key, acc) */
} PolyState_t;

/**
 * @brief Generates the POLY mac according to RFC 7539 section 2.5.1
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t PolyMacCalc(mbedtls_poly_key  key,        /*!< [in] Poniter to 256 bits of KEY. */
            const uint8_t       *pAddData,  /*!< [in] Optional - pointer to additional data if any */
            size_t          addDataSize,    /*!< [in] The size of the additional data */
            const uint8_t       *pDataIn,   /*!< [in] Pointer to data buffer to calculate MAC on */
            size_t          dataInSize, /*!< [in] The size of the additional data */
            mbedtls_poly_mac        macRes,     /*!< [out] The calculated MAC */
            bool     isPolyAeadMode);  /*!< [in] Boolean indicating if the Poly MAC operation is part of AEAD or just poly */

#ifdef __cplusplus
}
#endif

#endif  /* POLY_H */
