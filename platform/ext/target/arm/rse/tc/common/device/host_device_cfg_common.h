/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

#ifndef __HOST_DEVICE_CFG_COMMON_H__
#define __HOST_DEVICE_CFG_COMMON_H__

/**
 * \file host_device_cfg_common.h
 * \brief
 * This is the device configuration file with only used peripherals
 * defined and configured via the secure and/or non-secure base address.
 */

#define DEFAULT_UART_CONTROL 0
#define DEFAULT_UART_BAUDRATE  115200

#ifdef RSE_USE_HOST_UART
/* ARM UART PL011 */
#define UART0_PL011_S
#define UART0_PL011_NS
#endif /* RSE_USE_HOST_UART */

#ifdef RSE_USE_LOCAL_UART
/* ARM UART PL011 */
#define UART0_LOCAL_PL011_S
#define UART0_LOCAL_PL011_NS
#endif

/*External Intel Strata Flash Device*/
#define SPI_STRATAFLASHJ3_S

/* CFI Controller */
#define CFI_S

/* MHU */
#define MHU0_S
#define MHU1_S

#define MHU_RSE_TO_AP_MONITOR_DEV MHU0_SENDER_DEV_S
#define MHU_AP_MONITOR_TO_RSE_DEV MHU0_RECEIVER_DEV_S
#define MHU_RSE_TO_AP_NS_DEV      MHU1_SENDER_DEV_S
#define MHU_AP_NS_TO_RSE_DEV      MHU1_RECEIVER_DEV_S

#endif  /* __HOST_DEVICE_CFG_COMMON_H__ */
