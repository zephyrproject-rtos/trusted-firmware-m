/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_init.h"

#include "cc3xx_dev.h"
#include "cc3xx_engine_state.h"
#include <assert.h>
#include "cc3xx_rng.h"

static void check_features(void)
{
    /* Check for hashing support */
#if defined (CC3XX_CONFIG_HASH_SHA224_ENABLE) || defined(CC3XX_CONFIG_HASH_SHA256_ENABLE)
    assert(P_CC3XX->host_rgf.host_boot & (1 << 17)); /* HASH_EXISTS_LOCAL */
    assert(P_CC3XX->host_rgf.host_boot & (1 << 15)); /* SHA_256_PRSNT_LOCAL */
#endif /* defined (CC3XX_CONFIG_HASH_SHA224_ENABLE) || CC3XX_CONFIG_HASH_SHA256_ENABLE */

#ifdef CC3XX_CONFIG_AES_CTR_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
    assert(P_CC3XX->host_rgf.host_boot & (1 << 25)); /* CTR_EXISTS_LOCAL */
    assert(P_CC3XX->aes.aes_hw_flags & (1 << 3)); /* CTR_EXISTS */
#endif

#ifdef CC3XX_CONFIG_AES_ECB_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
#endif

#ifdef CC3XX_CONFIG_AES_CBC_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
#endif

#ifdef CC3XX_CONFIG_AES_GCM_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
#endif

#ifdef CC3XX_CONFIG_AES_CMAC_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
    assert(P_CC3XX->host_rgf.host_boot & (1 << 21)); /* AES_CMAC_EXISTS_LOCAL */
#endif

#ifdef CC3XX_CONFIG_AES_CCM_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 30)); /* AES_EXISTS_LOCAL */
    assert(P_CC3XX->host_rgf.host_boot & (1 << 22)); /* AES_CCM_EXISTS_LOCAL */
#endif

#ifdef CC3XX_CONFIG_AES_TUNNELLING_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 27)); /* TUNNELLING_ENB_LOCAL */
    assert(P_CC3XX->aes.aes_hw_flags & (1 << 10)); /* AES_TUNNEL_EXISTS */
#endif

#ifdef CC3XX_CONFIG_CHACHA_ENABLE
    assert(!P_CC3XX->host_rgf.host_remove_chacha_engine);
    assert(P_CC3XX->chacha.chacha_hw_flags & 1); /* CHACHA_EXISTS */
#endif

#ifdef CC3XX_CONFIG_RNG_ENABLE
    assert(P_CC3XX->host_rgf.host_boot & (1 << 11)); /* RNG_EXISTS_LOCAL */
#endif
}

static cc3xx_err_t setup_dfa_countermeasures(void)
{
#ifdef CC3XX_CONFIG_HW_VERSION_CC310
    /* CC310 has no AO interface where DFA could be disabled */
    return CC3XX_ERR_SUCCESS;
#endif /* CC3XX_CONFIG_HW_VERSION_CC310 */

#ifdef CC3XX_CONFIG_DFA_MITIGATIONS_ENABLE
    const uint32_t dfa_is_supported = P_CC3XX->aes.aes_hw_flags & (0x1 << 12);
    uint32_t lock_dfa_enabled = dfa_is_supported;
#else
    /* If the DFA countermeasures are not enabled, the setup just locks them to false
     * regardless of the HW being capable of DFA support or not
     */
    uint32_t lock_dfa_enabled = false;
#endif

#ifndef CC3XX_CONFIG_AES_TUNNELLING_ENABLE
    /* If the AES DFA countermeasures are supported, enable them. */
    if (lock_dfa_enabled) {
        P_CC3XX->ao.host_ao_lock_bits |= 0b1U << 7; /* Set HOST_FORCE_DFA_ENABLE */
    } else {
#else
    /* If tunnelling is enabled then the DFA countermeasures will need to be
     * switched off while it is in use. Because of this, FORCE_DFA_ENABLE needs
     * to be switched off.
     */
    {
    (void)lock_dfa_enabled;
#endif /* CC3XX_CONFIG_AES_TUNNELLING_ENABLE */
        P_CC3XX->ao.host_ao_lock_bits &= ~(0b1U << 7); /* Unset HOST_FORCE_DFA_ENABLE */
    }
    P_CC3XX->ao.host_ao_lock_bits |= 0b1U << 8; /* Set HOST_DFA_ENABLE_LOCK */

    return CC3XX_ERR_SUCCESS;
}

#ifdef CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE
static cc3xx_err_t setup_dpa_countermeasures(void)
{
    cc3xx_err_t err;
    uint8_t aes_rbg_seed;

    /* Some countermeasures are supported only for certain revisions */
    switch (P_CC3XX->id.peripheral_id_0) {
    case 0xC1:
        P_CC3XX->aes.aes_dummy_rounds_enable = 0x1;
        while (!P_CC3XX->aes.aes_rbg_seeding_rdy) {}
        err = cc3xx_lowlevel_rng_get_random((uint8_t *)&aes_rbg_seed, 1,
                                            CC3XX_RNG_DRBG);
        if (err != CC3XX_ERR_SUCCESS) {
            return err;
        }
        P_CC3XX->aes.aes_rbg_seed = aes_rbg_seed;
        break;
    }

    return CC3XX_ERR_SUCCESS;
}
#endif /* CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE */

cc3xx_err_t cc3xx_lowlevel_init(void)
{
    cc3xx_err_t err;

    /* If on a debug build, check that the CC3XX has all the features that have
     * been chosen by config */
    check_features();

    /* Configure entire system to little endian */
    P_CC3XX->host_rgf.host_rgf_endian = 0x0U;

    err = setup_dfa_countermeasures();
    if (err != CC3XX_ERR_SUCCESS) {
        return err;
    }

#ifdef CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE
    err = setup_dpa_countermeasures();
    if (err != CC3XX_ERR_SUCCESS) {
        return err;
    }
#endif /* CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE */

    /* Set the AHB to issue secure, privileged data access transactions */
    P_CC3XX->ahb.ahbm_hprot = 0b11U;
    P_CC3XX->ahb.ahbm_hnonsec = 0b00U;

    /* Reset engine to PASSTHROUGH / None */
    cc3xx_engine_in_use = CC3XX_ENGINE_NONE;
    P_CC3XX->cc_ctl.crypto_ctl = CC3XX_ENGINE_NONE;

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_lowlevel_uninit(void)
{
    return CC3XX_ERR_SUCCESS;
}
