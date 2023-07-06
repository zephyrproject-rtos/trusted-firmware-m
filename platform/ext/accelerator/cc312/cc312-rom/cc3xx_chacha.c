/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_chacha.h"

#include "cc3xx_dev.h"
#include "cc3xx_engine_state.h"
#include "cc3xx_stdlib.h"

#include <assert.h>

struct cc3xx_chacha_state_t chacha_state;

static void set_iv(const uint32_t *iv)
{
    if (chacha_state.iv_is_96_bit) {
        P_CC3XX->chacha.chacha_block_cnt_msb = iv[0];
        iv += 1;
    }
    P_CC3XX->chacha.chacha_iv[0] = iv[0];
    P_CC3XX->chacha.chacha_iv[1] = iv[1];
}

static void get_iv(uint32_t *iv)
{
    if (chacha_state.iv_is_96_bit) {
        iv[0] = P_CC3XX->chacha.chacha_block_cnt_msb;
        iv += 1;
    }
    iv[0] = P_CC3XX->chacha.chacha_iv[0];
    iv[1] = P_CC3XX->chacha.chacha_iv[1];
}

static void set_ctr(const uint64_t ctr)
{
    if (!chacha_state.iv_is_96_bit) {
        P_CC3XX->chacha.chacha_block_cnt_msb = ctr >> 32;
    }
    P_CC3XX->chacha.chacha_block_cnt_lsb = (uint32_t)ctr;
}

static uint64_t get_ctr(void)
{
    uint64_t ctr = 0;

    if (!chacha_state.iv_is_96_bit) {
        ctr = (uint64_t)P_CC3XX->chacha.chacha_block_cnt_msb << 32;
    }
    ctr |= P_CC3XX->chacha.chacha_block_cnt_lsb;

    return ctr;
}

static inline void set_key(const uint32_t *key)
{
    size_t idx;

#ifdef CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE
    (void)idx;

    cc3xx_secure_word_copy(P_CC3XX->chacha.chacha_key, key, 7);
    P_CC3XX->chacha.chacha_key[7] = key[7];
#else
    for (idx = 0; idx < 8; idx++) {
        P_CC3XX->chacha.chacha_key[idx] = key[idx];
    }
#endif /* CC3XX_CONFIG_DPA_MITIGATIONS_ENABLE */
}

void chacha_init_from_state(void)
{
    P_CC3XX->misc.chacha_clk_enable = 0x1U;

    cc3xx_set_engine(CC3XX_ENGINE_CHACHA);

    /* This is the block size */
    cc3xx_dma_set_buffer_size(64);

    set_key(chacha_state.key);

    P_CC3XX->chacha.chacha_control_reg = 0x0U;

    while(P_CC3XX->chacha.chacha_busy) {}

    /* Set to output the poly1305 key */
    P_CC3XX->chacha.chacha_control_reg |= (chacha_state.mode & 0b1) << 2;

    if (chacha_state.iv_is_96_bit) {
        /* 96 bit IVs use a special case */
        P_CC3XX->chacha.chacha_control_reg |= 0b1 << 10;
    }
}

cc3xx_err_t cc3xx_chacha20_init(cc3xx_chacha_direction_t direction,
                                cc3xx_chacha_mode_t mode,
                                const uint32_t *key,
                                uint64_t initial_counter,
                                const uint32_t *iv, size_t iv_len)
{
    /* TODO implement poly1305 */
    if (mode == CC3XX_CHACHA_MODE_CHACHA_POLY1305) {
        return CC3XX_ERR_NOT_IMPLEMENTED;
    }

    cc3xx_chacha20_uninit();

    chacha_state.direction = direction;
    chacha_state.mode = mode;

    if (iv_len == 12) {
        chacha_state.iv_is_96_bit = true;
    }

    memcpy(chacha_state.key, key, 32);

    chacha_init_from_state();

    set_iv(iv);
    set_ctr(initial_counter);

    /* Init new message from host. This must be the last thing done before data
     * is input.
     */
    P_CC3XX->chacha.chacha_control_reg |= 0b1 << 1;

    return CC3XX_ERR_SUCCESS;
}

#ifdef CC3XX_CONFIG_CHACHA_RESTARTABLE_ENABLE
void cc3xx_chacha20_get_state(struct cc3xx_chacha_state_t *state)
{
    memcpy(state, &chacha_state, sizeof(struct cc3xx_chacha_state_t));
    memcpy(&state->dma_state, &dma_state, sizeof(dma_state));

    get_iv(state->iv);
    state->counter = get_ctr();
}

cc3xx_err_t cc3xx_chacha20_set_state(const struct cc3xx_chacha_state_t *state)
{
    memcpy(&chacha_state, state, sizeof(struct cc3xx_chacha_state_t));
    memcpy(&dma_state, &state->dma_state, sizeof(dma_state));

    chacha_init_from_state();

    set_iv(state->iv);
    set_ctr(state->counter);

    /* Init new message from host. This must be the last thing done before data
     * is input.
     */
    P_CC3XX->chacha.chacha_control_reg |= 0b1 << 1;
}
#endif /* CC3XX_CONFIG_CHACHA_RESTARTABLE_ENABLE */

void cc3xx_chacha20_set_output_buffer(uint8_t *out, size_t out_len)
{
    cc3xx_dma_set_output(out, out_len);
}

void cc3xx_chacha20_update_authed_data(const uint8_t* in, size_t in_len)
{
    /* TODO implement poly1305 */
}

cc3xx_err_t cc3xx_chacha20_update(const uint8_t* in, size_t in_len)
{
    return cc3xx_dma_buffered_input_data(in, in_len, true);
}

cc3xx_err_t cc3xx_chacha20_finish(uint32_t *tag)
{
    /* Check alignment */
    assert(((uintptr_t)tag & 0b11) == 0);

    cc3xx_dma_flush_buffer(false);

    return CC3XX_ERR_SUCCESS;
}

void cc3xx_chacha20_uninit(void)
{
    uint32_t zero_iv[3] = {0};
    memset(&chacha_state, 0, sizeof(chacha_state));

    set_ctr(0);
    set_iv(zero_iv);

    P_CC3XX->chacha.chacha_control_reg = 0;
    P_CC3XX->misc.chacha_clk_enable = 0;

    cc3xx_dma_uninit();
}
