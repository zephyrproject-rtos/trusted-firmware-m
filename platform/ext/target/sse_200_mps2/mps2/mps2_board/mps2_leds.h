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

#ifndef __MPS2_BOARD_LEDS_H__
#define __MPS2_BOARD_LEDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum mps2_led_t {
  MPS2_FPGAIO_LED,
  MPS2_SCC_LED
};

enum mps2_led_name_t {
  MPS2_FPGAIO_USER_LED0,
  MPS2_FPGAIO_USER_LED1,
  MPS2_SCC_LED1,
  MPS2_SCC_LED2,
  MPS2_SCC_LED3,
  MPS2_SCC_LED4,
  MPS2_SCC_LED5,
  MPS2_SCC_LED6,
  MPS2_SCC_LED7,
  MPS2_SCC_LED8
};

/**
 * \brief Gets number of available FPGA LEDs.
 *
 * \param[in] led_type  FPGA LEDs type (\ref mps2_led_t).
 *
 * \return Number of available LEDs.
 */
uint32_t mps2_get_nbr_leds(enum mps2_led_t led_type);

/**
 * \brief Sets FPGA LEDs state.
 *
 * \param[in] led_type  FPGA LEDs type (\ref mps2_led_t).
 * \param[in] leds_val  FPGA LEDs value.
*/
void mps2_set_leds(enum mps2_led_t led_type, uint32_t leds_val);

/**
 * \brief Switchs on the given FPGA LED.
 *
 * \param[in] led_type  FPGA LEDs type (\ref mps2_led_t).
 * \param[in] led_id    FPGA LED Id (\ref mps2_led_name_t).
*/
void mps2_led_on(enum mps2_led_t led_type, enum mps2_led_name_t led_id);

/**
 * \brief Switchs off the given FPGA LED.
 *
 * \param[in] led_type  FPGA LEDs type (\ref mps2_led_t).
 * \param[in] led_id    FPGA LED Id (\ref mps2_led_name_t).
*/
void mps2_led_off(enum mps2_led_t led_type, enum mps2_led_name_t led_id);

#ifdef __cplusplus
}
#endif
#endif /* __MPS2_BOARD_LEDS_H__ */
