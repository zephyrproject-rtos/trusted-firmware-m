/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */


#include "tfm_plat_test.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

#define TIMER0_IRQ0_NUM 0
#define TIMER_MS 1000
#define TIMER_DELAY_US (500 * TIMER_MS)

extern void TFM_TIMER0_IRQ_Handler(void);

#ifdef TFM_PARTITION_SLIH_TEST
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_SLIH_TEST", "APP-ROT")
#elif defined(TFM_PARTITION_FLIH_TEST)
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_FLIH_TEST", "APP-ROT")
#endif
void tfm_plat_test_secure_timer_set_alarm_in_us(uint32_t delay_us)
{
    /* Load timer */
    uint64_t target = timer0_hw->timerawl + delay_us;

    /* Write the lower 32 bits of the target time to the alarm which will
       arm it */
    timer0_hw->alarm[TIMER0_IRQ0_NUM] = (uint32_t) target;
}

void tfm_plat_test_secure_timer_irq_handler(void)
{
    TFM_TIMER0_IRQ_Handler();
    tfm_plat_test_secure_timer_set_alarm_in_us(TIMER_DELAY_US);
}

void tfm_plat_test_secure_timer_start(void)
{
    /* Enable Timer0_0 interrupt */
    hw_set_bits(&timer0_hw->inte, 1u << TIMER0_IRQ0_NUM);
    tfm_plat_test_secure_timer_set_alarm_in_us(TIMER_DELAY_US);
}

void tfm_plat_test_secure_timer_clear_intr(void)
{
    hw_clear_bits(&timer0_hw->intr, 1u << TIMER0_IRQ0_NUM);
}

void tfm_plat_test_secure_timer_stop(void)
{
    /* Disable Timer0_0 interrupt */
    hw_clear_bits(&timer0_hw->inte, 1u << TIMER0_IRQ0_NUM);
}

void tfm_plat_test_non_secure_timer_start(void)
{
}

void tfm_plat_test_non_secure_timer_stop(void)
{
}
