/*
 * Copyright (c) 2020 Linaro Limited
 *
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_SPE_SHM_OPEN_AMP_H_
#define TFM_SPE_SHM_OPEN_AMP_H_

#include "region_defs.h"

#define VDEV_STATUS_ADDR    (OPENAMP_SE_SHARED_MEMORY_START_ADDR)
#define VDEV_STATUS_SIZE    (0x400) // 1 KB
#define SHM_START_VIRT_ADDR (OPENAMP_SE_SHARED_MEMORY_START_ADDR + VDEV_STATUS_SIZE)
#define SHM_START_PHY_ADDR  (OPENAMP_HOST_SHARED_MEMORY_START_ADDR + VDEV_STATUS_SIZE)
#define SHM_SIZE            (0x7c00) // 31 KB
#define SHM_DEVICE_NAME     "cvm.shm"

#define VRING_COUNT         2
#define VRING_TX_ADDRESS    (VDEV_STATUS_ADDR + SHM_SIZE)
#define VRING_RX_ADDRESS    (VDEV_STATUS_ADDR + SHM_SIZE - VDEV_STATUS_SIZE)
#define VRING_ALIGNMENT     4
#define VRING_SIZE          8

/*
 * The tf-m can only accept MAX_MESSAGES at a given time.
 * The Host should set RPMSG_BUFFER_SIZE accrodingly
 * such that tf-m does not recieve more than
 * TFM_MAX_MESSAGES messages.
 * Changing this macro DOES NOT increase TF-M capabilities
 * to handle more messages.
 */
#define TFM_MAX_MESSAGES            (32)

#endif /* TFM_SPE_SHM_OPEN_AMP_H_ */
