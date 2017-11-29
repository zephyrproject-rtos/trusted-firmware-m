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

#include "mps2_time.h"
#include "smm_mps2.h"

#define TICKS_PER_US  (SystemCoreClock / 1000000U)
#define TICKS_PER_MS  (SystemCoreClock / 1000U)

uint32_t mps2_get_ticks(enum mps2_clk_t clk)
{
    switch (clk) {
    case MPS2_CLK_1HZ:
        return MPS2_FPGAIO->CLK1HZ;
    case MPS2_CLK_100HZ:
        return MPS2_FPGAIO->CLK100HZ;
    case MPS2_CLK_25MHZ:
        return MPS2_FPGAIO->COUNTER;
    /* default: Compiler check */
    }
}

void mps2_sleepms(uint32_t msec)
{
    uint32_t end;
    uint32_t start;

    start = MPS2_FPGAIO->COUNTER;
    end   = start + (msec * TICKS_PER_MS);

    if (end >= start) {
        while (MPS2_FPGAIO->COUNTER >= start && MPS2_FPGAIO->COUNTER < end){};
    } else {
        while (MPS2_FPGAIO->COUNTER >= start){};
        while (MPS2_FPGAIO->COUNTER < end){};
    }
}

void mps2_sleepus(uint32_t usec)
{
    uint32_t end;
    uint32_t start;

    start = MPS2_FPGAIO->COUNTER;
    end   = start + (usec * TICKS_PER_US);

    if (end >= start) {
        while (MPS2_FPGAIO->COUNTER >= start && MPS2_FPGAIO->COUNTER < end){};
    } else {
        while (MPS2_FPGAIO->COUNTER >= start){};
        while (MPS2_FPGAIO->COUNTER < end){};
    }
}

void mps2_sleeps(uint32_t sec)
{
    uint32_t end;
    uint32_t start;

    start = MPS2_FPGAIO->CLK1HZ;
    end   = start + sec;

    if (end >= start) {
        while (MPS2_FPGAIO->CLK1HZ >= start && MPS2_FPGAIO->CLK1HZ < end){};
    } else {
        while (MPS2_FPGAIO->CLK1HZ >= start){};
        while (MPS2_FPGAIO->CLK1HZ < end){};
    }
}
