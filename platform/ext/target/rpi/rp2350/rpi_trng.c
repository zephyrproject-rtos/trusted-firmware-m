/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "psa/crypto.h"
#include "pico/rand.h"
#include <string.h>

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
psa_status_t mbedtls_psa_external_get_random(mbedtls_psa_external_random_context_t *context,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)
{
    size_t i = 0;
    size_t copy_size = 0;
    size_t remaining_size = 0;

    uint64_t tmp_trn;

    (void) context;

    while (i < output_size) {
        remaining_size = output_size - i;
        copy_size = (remaining_size > 8) ? 8 : remaining_size;

        tmp_trn = get_rand_64();
        memcpy(output + i, (uint8_t *)(&tmp_trn), copy_size);
        i += copy_size;
    }

    *output_length = output_size;

    return PSA_SUCCESS;
}
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
