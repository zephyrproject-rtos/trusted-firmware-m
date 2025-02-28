
/*
 * Copyright (c) 2023, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_KDF_H
#define CC3XX_KDF_H

#include "cc3xx_error.h"
#include "cc3xx_aes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                     This is a counter-mode KDF complying with NIST
 *                            SP800-108r1-upd1 where the PRF is the AES cipher
 *                            in CMAC mode of operation as per NIST SP800-38B.
 *
 * @note                      This function only outputs keys whose size is a
 *                            multiple of the output size of the AES-CMAC
 *                            operation (16 bytes, i.e. the AES block size).
 *
 * @note                      This is a NIST SP800-108r1 counter mode KDF with
 *                            CMAC as the PRF (pseudo-random function), using the
 *                            modified input [i]2 || Label || 0x00 || Context || [L]2 || K(0)
 *                            construction where
 *                            K(0) = PRF(K_in, Label || 0x00 || Context || [L]2), and [i]2 is
 *                            a counter variable initialised to 1. This assumes that r,
 *                            the length in bits of the encoding of i is 32 bits. It sets the
 *                            encoding of L as well on 32-bits. This implements the
 *                            enhanced input construction described in SP800-108r1-upd1 section 4.1.
 *                            [i]2 and [L]2 are both in big endian, [L]2 is the output length in bits
 *
 * @param[in]  key_id         Which user/hardware key should be used.
 * @param[in]  key            If key_id is set to CC3XX_AES_KEY_ID_USER_KEY,
 *                            this buffer contains the key material.
 * @param[in]  key_size       The size of the key being used.
 * @param[in]  label          The label to input into the derivation operation.
 * @param[in]  label_length   The length of the label.
 * @param[in]  context        The context to input into the derivation operation.
 * @param[in]  context_length The length of the context.
 * @param[out] output_key     The buffer to output the key into.
 * @param[in]  out_length     The size in bytes of the key to derive.
 *
 * @return                    CC3XX_ERR_SUCCESS on success, another
 *                            cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_kdf_cmac(
    cc3xx_aes_key_id_t key_id, const uint32_t *key,
    cc3xx_aes_keysize_t key_size,
    const uint8_t *label, size_t label_length,
    const uint8_t *context, size_t context_length,
    uint32_t *output_key, size_t out_length);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_KDF_H */
