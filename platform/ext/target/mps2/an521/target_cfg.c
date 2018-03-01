/*
 * Copyright (c) 2017-2018 ARM Limited
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

#include <arm_cmse.h>

#include "cmsis.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "platform_retarget_dev.h"
#include "region_defs.h"
#include "tfm_secure_api.h"

/*
 * This function pointer is meant to only hold non secure function pointers.
 * It will be turned into a non-secure one (LSB cleared) before being called
 * whatever happens anyway (unless cast to another function pointer type).
 * Registers will be cleared before branching so that no information leaks
 * from secure to non-secure world.
 */
typedef void __attribute__((cmse_nonsecure_call)) (*nsfptr_t) (void);

/* Allows software, via SAU, to define the code region as a NSC */
#define NSCCFG_CODENSC  1

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_SRAM1_MPC, Driver_SRAM2_MPC;

/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START (0x40000000)
#define PERIPHERALS_BASE_NS_END   (0x4FFFFFFF)

void configure_ns_code()
{
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = (NS_CODE_START);

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = *((uint32_t*)(NS_CODE_START));
    __TZ_set_MSP_NS(ns_msp);
}

void jump_to_ns_code()
{
    /* The entry contains address of the Reset_handler (CMSIS-CORE) function */
    uint32_t entry_ptr = *((uint32_t*)(NS_CODE_START + 4));

    /* Clears LSB of the function address to indicate the function-call
       will perform the switch from secure to non-secure */
    nsfptr_t ns_entry = (nsfptr_t) cmse_nsfptr_create(entry_ptr);

    /* All changes made to memory will be effective after this point */
    __DSB();
    __ISB();

    /* Calls the non-secure Reset_Handler to jump to the non-secure binary */
    ns_entry();
}

void enable_fault_handlers(void)
{
    /* Enables BUS, MEM, USG and Secure faults */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
                  | SCB_SHCSR_BUSFAULTENA_Msk
                  | SCB_SHCSR_MEMFAULTENA_Msk
                  | SCB_SHCSR_SECUREFAULTENA_Msk;

    /* CFSR register setting to enable precise errors */
    SCB->CFSR |= SCB_CFSR_PRECISERR_Msk;
}

/*------------------- NVIC interrupt target state to NS configuration ----------*/
void nvic_interrupt_target_state_cfg()
{
    /* Target every interrupt to NS; unimplemented interrupts will be WI */
    for (uint8_t i=0; i<sizeof(NVIC->ITNS)/sizeof(NVIC->ITNS[0]); i++) {
        NVIC->ITNS[i] = 0xFFFFFFFF;
    }

    /* Make sure that MPC and PPC are targeted to S state */
    NVIC_ClearTargetState(MPC_IRQn);
    NVIC_ClearTargetState(PPC_IRQn);

    /* UART1 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(UARTRX1_IRQn);
    NVIC_ClearTargetState(UARTTX1_IRQn);
    NVIC_ClearTargetState(UART1_IRQn);
}

/*------------------- NVIC interrupt enabling for S peripherals ----------------*/
void nvic_interrupt_enable()
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;

    /* MPC interrupt enabling */
    Driver_SRAM1_MPC.EnableInterrupt();
    Driver_SRAM2_MPC.EnableInterrupt();
    NVIC_EnableIRQ(MPC_IRQn);

    /* PPC interrupt enabling */
    /* Clear pending PPC interrupts */
    /* In the PPC configuration function, we have used the Non-Secure
     * Privilege Control Block to grant unprivilged NS access to some
     * peripherals used by NS. That triggers a PPC0 exception as that
     * register is meant for NS privileged access only. Clear it here
     */
    spctrl->secppcintclr = CMSDK_APB_PPC0_INT_POS_MASK;

    /* Enable PPC interrupts for APB PPC */
    spctrl->secppcinten |= CMSDK_APB_PPC0_INT_POS_MASK;
    spctrl->secppcinten |= CMSDK_APB_PPC1_INT_POS_MASK;
    spctrl->secppcinten |= CMSDK_APB_PPCEXP0_INT_POS_MASK;
    spctrl->secppcinten |= CMSDK_APB_PPCEXP1_INT_POS_MASK;
    spctrl->secppcinten |= CMSDK_APB_PPCEXP2_INT_POS_MASK;
    spctrl->secppcinten |= CMSDK_APB_PPCEXP3_INT_POS_MASK;
    NVIC_EnableIRQ(PPC_IRQn);
}

/*------------------- SAU/IDAU configuration functions -------------------------*/

void sau_and_idau_cfg(void)
{
    /* Enables SAU */
    TZ_SAU_Enable();

    /* Configures SAU regions to be non-secure */
    SAU->RNR  = TFM_NS_REGION_CODE;
    SAU->RBAR = (NS_PARTITION_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (NS_PARTITION_LIMIT & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    SAU->RNR  = TFM_NS_REGION_DATA;
    SAU->RBAR = (NS_DATA_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (NS_DATA_LIMIT & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    /* Configures veneers region to be non-secure callable */
    SAU->RNR  = TFM_NS_REGION_VENEER;
    SAU->RBAR = (CMSE_VENEER_REGION_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (CMSE_VENEER_REGION_LIMIT & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk
                | SAU_RLAR_NSC_Msk;

    /* Configure the peripherals space */
    /* Only UART1 is configured as a secure peripheral */
    SAU->RNR  = TFM_NS_REGION_PERIPH_1;
    SAU->RBAR = (PERIPHERALS_BASE_NS_START & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = ((UART1_BASE_NS-1) & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk;

    /* The UART1 range is considered as a (secure) gap */

    SAU->RNR  = TFM_NS_REGION_PERIPH_2;
    SAU->RBAR = (UART2_BASE_NS & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (PERIPHERALS_BASE_NS_END & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk;

    /* Allows SAU to define the code region as a NSC  */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    spctrl->nsccfg |= NSCCFG_CODENSC;
}

/*------------------- Memory configuration functions -------------------------*/

void mpc_init_cfg(void)
{
    Driver_SRAM1_MPC.Initialize();
    Driver_SRAM1_MPC.ConfigRegion(NS_PARTITION_START,
                                  NS_PARTITION_LIMIT,
                                  ARM_MPC_ATTR_NONSECURE);

    Driver_SRAM2_MPC.Initialize();
    Driver_SRAM2_MPC.ConfigRegion(NS_DATA_START, NS_DATA_LIMIT,
                                  ARM_MPC_ATTR_NONSECURE);

    /* Lock down the MPC configuration */
    Driver_SRAM1_MPC.LockDown();
    Driver_SRAM2_MPC.LockDown();

    /* Add barriers to assure the MPC configuration is done before continue
     * the execution. */
    __DSB();
    __ISB();
}

/*------------------- PPC configuration functions -------------------------*/

void ppc_init_cfg(void)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    struct nspctrl_def* nspctrl = CMSDK_NSPCTRL;

    /* Grant non-secure access to peripherals in the PPC0
     * (timer0 and 1, dualtimer, watchdog, mhu 0 and 1) */
    spctrl->apbnsppc0 |= (1U << CMSDK_TIMER0_APB_PPC_POS);
    spctrl->apbnsppc0 |= (1U << CMSDK_TIMER1_APB_PPC_POS);
    spctrl->apbnsppc0 |= (1U << CMSDK_DTIMER_APB_PPC_POS);
    spctrl->apbnsppc0 |= (1U << CMSDK_MHU0_APB_PPC_POS);
    spctrl->apbnsppc0 |= (1U << CMSDK_MHU1_APB_PPC_POS);
    /* Grant non-secure access to S32K Timer in PPC1*/
    spctrl->apbnsppc1 |= (1U << CMSDK_S32K_TIMER_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP1 */
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI0_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI1_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI2_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI3_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI4_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_UART0_APB_PPC_POS);
    /* Do not do it for UART1 as it's a Secure peripheral */
    spctrl->apbnsppcexp1 |= (1U << CMSDK_UART2_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_UART3_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_UART4_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_I2C0_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_I2C1_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_I2C2_APB_PPC_POS);
    spctrl->apbnsppcexp1 |= (1U << CMSDK_I2C3_APB_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP2 */
    spctrl->apbnsppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS);
    spctrl->apbnsppcexp2 |= (1U << CMSDK_FPGA_AUDIO_PPC_POS);
    spctrl->apbnsppcexp2 |= (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Grant non-secure access to all peripherals on AHB EXP:
     * Make sure that all possible peripherals are enabled by default
     */
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_VGA_PPC_POS);
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_GPIO0_PPC_POS);
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_GPIO1_PPC_POS);
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_GPIO2_PPC_POS);
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_GPIO3_PPC_POS);

    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA0_PPC_POS);
    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA1_PPC_POS);
    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA2_PPC_POS);
    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA3_PPC_POS);

    /* in NS, grant un-privileged for UART0 */
    nspctrl->apbnspppcexp1 |= (1U << CMSDK_UART0_APB_PPC_POS);

    /* in NS, grant un-privileged access for LEDs */
    nspctrl->apbnspppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS);
    nspctrl->apbnspppcexp2 |= (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Configure the response to a security violation as a
     * bus error instead of RAZ/WI */
    spctrl->secrespcfg |= 1U;
}

void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint16_t pos)
{
    /* Clear NS flag for peripheral to prevent NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] |= (1U << pos);
}

void ppc_configure_to_secure(enum ppc_bank_e bank, uint16_t pos)
{
    /* Clear NS flag for peripheral to prevent NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] &= ~(1U << pos);
}

void ppc_en_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] |= (1U << pos);
}

void ppc_clr_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] &= ~(1U << pos);
}

void ppc_clear_irq(void)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    /* Clear APC PPC EXP2 IRQ */
    spctrl->secppcintclr = CMSDK_APB_PPCEXP2_INT_POS_MASK;
}
