/*
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mps2_leds.h"
#include "smm_mps2.h"

#define MPS2_FPGAIO_NBR_LEDS  2U  /* Number of available MPS2 FPGAIO LEDs */
#define MPS2_SCC_NBR_LEDS     8U  /* Number of available MPS2 SCC LEDs */

uint32_t mps2_get_nbr_leds(enum mps2_led_t led_type)
{
    switch(led_type) {
    case MPS2_FPGAIO_LED:
        return MPS2_FPGAIO_NBR_LEDS;
    case MPS2_SCC_LED:
        return MPS2_SCC_NBR_LEDS;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all enumeration values are
     *           covered in the switch.*/
    }
}

void mps2_set_leds(enum mps2_led_t led_type, uint32_t leds_val)
{
    switch(led_type) {
    case MPS2_FPGAIO_LED:
        MPS2_FPGAIO->LED = ((1U << (MPS2_FPGAIO_NBR_LEDS)) - 1U) & leds_val;
        break;
    case MPS2_SCC_LED:
        MPS2_SCC->LEDS = ((1U << (MPS2_SCC_NBR_LEDS)) - 1U) & leds_val;
        break;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all enumeration values are
     *           covered in the switch.*/
    }
}

void mps2_led_on(enum mps2_led_t led_type, enum mps2_led_name_t led_id)
{
    switch(led_type) {
    case MPS2_FPGAIO_LED:
        if (led_id < MPS2_FPGAIO_NBR_LEDS) {
            MPS2_FPGAIO->LED |=  (1U << led_id);
        }
        break;
    case MPS2_SCC_LED:
        if (led_id < MPS2_SCC_NBR_LEDS) {
            MPS2_SCC->LEDS |=  (1U << led_id);
        }
        break;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all enumeration values are
     *           covered in the switch.*/
    }
}

void mps2_led_off(enum mps2_led_t led_type, enum mps2_led_name_t led_id)
{
    switch(led_type) {
    case MPS2_FPGAIO_LED:
        if (led_id < MPS2_FPGAIO_NBR_LEDS) {
            MPS2_FPGAIO->LED &= ~(1U << led_id);
        }
        break;
    case MPS2_SCC_LED:
        if (led_id < MPS2_SCC_NBR_LEDS) {
            MPS2_SCC->LEDS &= ~(1U << led_id);
        }
        break;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all enumeration values are
     *           covered in the switch.*/
    }
}
