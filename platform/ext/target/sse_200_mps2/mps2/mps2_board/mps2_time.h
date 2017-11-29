/*
 * Copyright (c) 2017 ARM Limited
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

#ifndef __MPS2_TIME_H__
#define __MPS2_TIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enum to define clocks in the system */
enum mps2_clk_t {
    MPS2_CLK_1HZ = 0,  /*!< 1Hz ticker */
    MPS2_CLK_100HZ,    /*!< 100Hz ticker */
    MPS2_CLK_25MHZ     /*!< 25MHz ticker */
};

/**
 * \brief  Return number of ticks of a clock
 *
 * \param[in] clk  Clock to get ticks from
 *
 * \return  Value of the counter
 */
uint32_t mps2_get_ticks(enum mps2_clk_t clk);

/**
 * \brief Sleep function to delay milliseconds.
 *
 * \param[in] msec  Time to sleep in msec
 *
 * \note Maximum measurable time with current implementation is 171s
 */
void mps2_sleepms(uint32_t msec);

/**
 * \brief Sleep function to delay microseconds.
 *
 * \param[in] usec  Time to sleep in usec
 *
 * \note Maximum measurable time with current implementation is 171s
 */
void mps2_sleepus(uint32_t usec);

/**
 * \brief Sleep function to delay in seconds.
 *
 * \param[in] sec  Time to sleep in seconds
 */
void mps2_sleeps(uint32_t sec);

#ifdef __cplusplus
}
#endif
#endif /* __MPS2_TIME_H__ */
