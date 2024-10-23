/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __PLATFORM_MULTICORE_H__
#define __PLATFORM_MULTICORE_H__

#include <stdint.h>

#include "hardware/structs/sio.h"

#define CORE1_S_READY           0x10
#define CORE1_NS_READY          0x20
#define CORE0_NS_READY          0x30

#define NS_MAILBOX_INIT         0x100
#define S_MAILBOX_READY         0x110

#define NOTIFY_FROM_CORE0       0x200
#define NOTIFY_FROM_CORE1       0x300

#define HALT_DOORBELL_MASK      (0x1UL << 0)
#define FLASH_DOORBELL_MASK     (0x1UL << 1)

#define UART_SPINLOCK_NUM       0
#define FLASH_SPINLOCK_NUM      1
#define MAILBOX_SPINLOCK_NUM    2

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define UART_SPINLOCK           (&sio_ns_hw->spinlock[UART_SPINLOCK_NUM])
#define FLASH_SPINLOCK          (&sio_hw->spinlock[FLASH_SPINLOCK_NUM])
#define MAILBOX_SPINLOCK        (&sio_ns_hw->spinlock[MAILBOX_SPINLOCK_NUM])
#else
#define UART_SPINLOCK           (&sio_hw->spinlock[UART_SPINLOCK_NUM])
#define MAILBOX_SPINLOCK        (&sio_hw->spinlock[MAILBOX_SPINLOCK_NUM])
#endif


bool multicore_ns_fifo_rvalid(void);
bool multicore_ns_fifo_wready(void);
void multicore_ns_fifo_push_blocking_inline(uint32_t data);
uint32_t multicore_ns_fifo_pop_blocking_inline(void);
extern volatile uint32_t CORE1_RUNNING;


#endif  /* __PLATFORM_MULTICORE_H__ */
