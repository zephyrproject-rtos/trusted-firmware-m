/*
 * Copyright (c) 2017-2018 ARM Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_retarget_dev.h
 * \brief The structure definitions in this file are exported based on the peripheral
 * definitions from device_cfg.h.
 * This retarget file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#ifndef __ARM_LTD_MUSCA_RETARGET_DEV_H__
#define __ARM_LTD_MUSCA_RETARGET_DEV_H__

#include "device_cfg.h"

/* ======= Includes generic driver headers ======= */
#include "mpc_sie200_drv.h"
#include "ppc_sse200_drv.h"
#include "arm_gpio_drv.h"
#include "timer_cmsdk.h"
#include "uart_pl011_drv.h"

/* ======= Peripheral configuration structure declarations ======= */
/* ARM SCC driver structures */
#ifdef ARM_SCC_S
extern struct arm_scc_dev_t ARM_SCC_DEV_S;
#endif
#ifdef ARM_SCC_NS
extern struct arm_scc_dev_t ARM_SCC_DEV_NS;
#endif

/* ARM GPIO driver structures */
#ifdef ARM_GPIO0_S
extern struct arm_gpio_dev_t ARM_GPIO0_DEV_S;
#endif
#ifdef ARM_GPIO0_NS
extern struct arm_gpio_dev_t ARM_GPIO0_DEV_NS;
#endif

/* ARM MPC SIE 200 driver structures */
#ifdef MPC_ISRAM0_S
extern struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S;
#endif
#ifdef MPC_ISRAM1_S
extern struct mpc_sie200_dev_t MPC_ISRAM1_DEV_S;
#endif
#ifdef MPC_ISRAM2_S
extern struct mpc_sie200_dev_t MPC_ISRAM2_DEV_S;
#endif
#ifdef MPC_ISRAM3_S
extern struct mpc_sie200_dev_t MPC_ISRAM3_DEV_S;
#endif
#ifdef MPC_CODE_SRAM1_S
extern struct mpc_sie200_dev_t MPC_CODE_SRAM1_DEV_S;
#endif
#ifdef MPC_CODE_SRAM1_NS
extern struct mpc_sie200_dev_t MPC_CODE_SRAM1_DEV_NS;
#endif
#ifdef MPC_QSPI_S
extern struct mpc_sie200_dev_t MPC_QSPI_DEV_S;
#endif
#ifdef MPC_QSPI_NS
extern struct mpc_sie200_dev_t MPC_QSPI_DEV_NS;
#endif

/* CMSDK Timer driver structures */
#ifdef CMSDK_TIMER0_S
extern struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_S;
#endif
#ifdef CMSDK_TIMER0_NS
extern struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_NS;
#endif

#ifdef CMSDK_TIMER1_S
extern struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_S;
#endif
#ifdef CMSDK_TIMER1_NS
extern struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_NS;
#endif

/* ARM UART PL011 driver structures */
#ifdef UART0_PL011_S
extern struct uart_pl011_dev_t UART0_DEV_S;
#endif
#ifdef UART0_PL011_NS
extern struct uart_pl011_dev_t UART0_DEV_NS;
#endif
#ifdef UART1_PL011_S
extern struct uart_pl011_dev_t UART1_DEV_S;
#endif
#ifdef UART1_PL011_NS
extern struct uart_pl011_dev_t UART1_DEV_NS;
#endif

/* ARM PPC driver structures */
#ifdef AHB_PPC0_S
extern struct ppc_sse200_dev_t AHB_PPC0_DEV_S;
#endif

#ifdef AHB_PPCEXP0_S
extern struct ppc_sse200_dev_t AHB_PPCEXP0_DEV_S;
#endif

#ifdef AHB_PPCEXP1_S
extern struct ppc_sse200_dev_t AHB_PPCEXP1_DEV_S;
#endif

#ifdef AHB_PPCEXP2_S
extern struct ppc_sse200_dev_t AHB_PPCEXP2_DEV_S;
#endif

#ifdef AHB_PPCEXP3_S
extern struct ppc_sse200_dev_t AHB_PPCEXP3_DEV_S;
#endif

#ifdef APB_PPC0_S
extern struct ppc_sse200_dev_t APB_PPC0_DEV_S;
#endif

#ifdef APB_PPC1_S
extern struct ppc_sse200_dev_t APB_PPC1_DEV_S;
#endif

#ifdef APB_PPCEXP0_S
extern struct ppc_sse200_dev_t APB_PPCEXP0_DEV_S;
#endif

#ifdef APB_PPCEXP1_S
extern struct ppc_sse200_dev_t APB_PPCEXP1_DEV_S;
#endif

#ifdef APB_PPCEXP2_S
extern struct ppc_sse200_dev_t APB_PPCEXP2_DEV_S;
#endif

#ifdef APB_PPCEXP3_S
extern struct ppc_sse200_dev_t APB_PPCEXP3_DEV_S;
#endif

#endif  /* __ARM_LTD_MUSCA_RETARGET_DEV_H__ */
