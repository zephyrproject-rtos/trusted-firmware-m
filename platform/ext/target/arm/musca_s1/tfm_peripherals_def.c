/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_peripherals_def.h"
#include "array.h"
#include <stddef.h>
#include <stdint.h>

/* Allowed named MMIO of this platform */
static const uintptr_t partition_named_mmio_list[] = {
    (uintptr_t)TFM_PERIPHERAL_TIMER0,
    (uintptr_t)TFM_PERIPHERAL_STD_UART,
#ifdef PSA_API_TEST_IPC
    (uintptr_t)FF_TEST_UART_REGION,
    (uintptr_t)FF_TEST_WATCHDOG_REGION,
    (uintptr_t)FF_TEST_NVMEM_REGION,
    (uintptr_t)FF_TEST_SERVER_PARTITION_MMIO,
    (uintptr_t)FF_TEST_DRIVER_PARTITION_MMIO,
#endif
};

/*
 * Platform MUSCA_S1 only has named MMIO.
 * If the platform has numbered MMIO, define them in another list.
 */

void get_partition_named_mmio_list(const uintptr_t **list, size_t *length) {
    *list = partition_named_mmio_list;
    *length = ARRAY_SIZE(partition_named_mmio_list);
}
