/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>

#include "uart_stdout.h"

#ifdef TEST_BL1_1
#include "bl1_1_suites.h"
#endif /* TEST_BL1_1 */

#ifdef TEST_BL1_2
#include "bl1_2_suites.h"
#endif /* TEST_BL1_2 */

int main(void)
{
    stdio_init();

#ifdef TEST_BL1_2
    run_bl1_2_testsuite();
#endif /* TEST_BL1_2 */
#ifdef TEST_BL1_1
    run_bl1_1_testsuite();
#endif /* TEST_BL1_1 */

    while(1){
        __asm volatile("wfi");
    }
}
