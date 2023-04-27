/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_KEY_DERIVATION_H__
#define __RSS_KEY_DERIVATION_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                     Get the boot state.

 * \param[out] state          The buffer to get the state into.
 * \param[in]  state_buf_len  The size of the state buffer.
 * \param[out] state_size     The size of the state.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_get_boot_state(uint8_t *state, size_t state_buf_len,
                       size_t *state_size);

/**
 * \brief                     Derive a VHUK seed.

 * \param[out] vhuk_seed         The buffer to derive the seed into.
 * \param[in]  vhuk_seed_buf_len The size of the seed buffer.
 * \param[out] vhuk_seed_size    The size of the seed.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_derive_vhuk_seed(uint8_t *vhuk_seed, size_t vhuk_seed_buf_len,
                         size_t *vhuk_seed_size);

/**
 * \brief                     Derive the CPAK seed, and lock in a KMU slot.

 * \param[in]  kmu_output_slot The slot to derive and lock the seed into.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_derive_cpak_seed(uint32_t kmu_output_slot);

/**
 * \brief                     Derive the DAK seed, and lock in a KMU slot.

 * \param[in]  kmu_output_slot The slot to derive and lock the seed into.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_derive_dak_seed(uint32_t kmu_output_slot);

/**
 * \brief                     Derive the VHUK, and lock in a KMU slot.

 * \param[in]  vhuk_seeds     A buffer containing the seed values.
 * \param[in]  vhuk_seeds_len The size of the vhuk_seeds buffer. This must be
 *                            RSS_AMOUNT * 32 in size.
 * \param[in]  kmu_output_slot The slot to derive and lock the seed into.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_derive_vhuk(const uint8_t *vhuk_seeds, size_t vhuk_seeds_len,
                    uint32_t kmu_output_slot);

/**
 * \brief                     Derive the session key, and lock in a KMU slot.

 * \param[in]  ivs             A buffer containing the iv values.
 * \param[in]  ivs_len         The size of the ivs buffer. This must be
 *                             RSS_AMOUNT * 32 in size.
 * \param[in]  kmu_output_slot The slot to derive and lock the seed into.
 *
 * \return                    0 on success, non-zero on error.
 */
int rss_derive_session_key(const uint8_t *ivs, size_t ivs_len,
                           uint32_t kmu_output_slot);

#ifdef __cplusplus
}
#endif

#endif /* __RSS_KEY_DERIVATION_H__ */
