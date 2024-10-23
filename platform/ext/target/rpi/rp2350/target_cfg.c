/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_hal_device_header.h"
#include "region_defs.h"
#include "target_cfg.h"
#include "tfm_plat_defs.h"
#include "tfm_peripherals_def.h"
#include "hardware/uart.h"
#include "region.h"
#include "hardware/regs/addressmap.h"
#include "hardware/structs/accessctrl.h"
#include "hardware/structs/dma.h"

#define REG_RW(addr) (*(volatile uint32_t *)(addr))

#define ACCESSCTRL_DBG (1U << 7)
#define ACCESSCTRL_DMA (1U << 6)
#define ACCESSCTRL_CORE1 (1U << 5)
#define ACCESSCTRL_CORE0 (1U << 4)
#define ACCESSCTRL_SP (1U << 3)
#define ACCESSCTRL_SU (1U << 2)
#define ACCESSCTRL_NSP (1U << 1)
#define ACCESSCTRL_NSU (1U << 0)

#define ACCESSCTRL_NS_PRIV (ACCESSCTRL_DBG | ACCESSCTRL_DMA |    \
                              ACCESSCTRL_CORE1 | ACCESSCTRL_CORE0 |\
                              ACCESSCTRL_SP | ACCESSCTRL_SU |      \
                              ACCESSCTRL_NSP)
#define ACCESSCTRL_S_UNPRIV_C0 (ACCESSCTRL_DBG | ACCESSCTRL_CORE0 |\
                                ACCESSCTRL_SP | ACCESSCTRL_SU)
/* Only grant access to Core0 when Multi-core topology is not in use */
#ifdef TFM_MULTI_CORE_TOPOLOGY
#define ACCESSCTRL_S_UNPRIV_C0_C1 (ACCESSCTRL_DBG | ACCESSCTRL_CORE0 |\
                                   ACCESSCTRL_CORE1 | ACCESSCTRL_SP |\
                                   ACCESSCTRL_SU)
#else
#define ACCESSCTRL_S_UNPRIV_C0_C1 ACCESSCTRL_S_UNPRIV_C0
#endif
#define ACCESSCTRL_S_PRIV_C0 (ACCESSCTRL_DBG | ACCESSCTRL_CORE0 |\
                              ACCESSCTRL_SP)


#ifdef CONFIG_TFM_USE_TRUSTZONE
REGION_DECLARE(Image$$, ER_VENEER, $$Base);
REGION_DECLARE(Image$$, VENEER_ALIGN, $$Limit);
#endif /* CONFIG_TFM_USE_TRUSTZONE */
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE_START, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE_END, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_APP_CODE_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_CODE_END, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);
#ifdef CONFIG_TFM_PARTITION_META
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_META_PTR_END, $$ZI$$Limit);
#endif /* CONFIG_TFM_PARTITION_META */

#define FF_TEST_NVMEM_REGION_START            0x2005E000
#define FF_TEST_NVMEM_REGION_END              0x2005E3FF
#define FF_TEST_SERVER_PARTITION_MMIO_START   0x2005E400
#define FF_TEST_SERVER_PARTITION_MMIO_END     0x2005E4FF
#define FF_TEST_DRIVER_PARTITION_MMIO_START   0x2005E600
#define FF_TEST_DRIVER_PARTITION_MMIO_END     0x2005E6FF

extern const struct memory_region_limits memory_regions;

struct platform_data_t tfm_peripheral_std_uart = {
        .periph_start = UART0_BASE,
        .periph_limit = UART0_BASE + 0x3FFF,
        /* Based on platform_data_t definition TF-M expects PPC to control
           security and privilege settings. There is no PPC on this platform.
           Using periph_ppc_mask to store accessctrl register index. */
        .periph_ppc_mask = AC_DO_NOT_CONFIGURE,
};

struct platform_data_t tfm_peripheral_timer0 = {
        .periph_start = TIMER0_BASE,
        .periph_limit = TIMER0_BASE + 0x3FFF,
        /* Based on platform_data_t definition TF-M expects PPC to control
           security and privilege settings. There is no PPC on this platform.
           Using periph_ppc_mask to store accessctrl register index. */
        .periph_ppc_mask = AC_TIMER0,
};

#ifdef PSA_API_TEST_IPC

/* Below data structure are only used for PSA FF tests, and this pattern is
 * definitely not to be followed for real life use cases, as it can break
 * security.
 */

struct platform_data_t
    tfm_peripheral_FF_TEST_UART_REGION = {
        .periph_start = UART1_BASE,
        .periph_limit = UART1_BASE + 0x3FFF,
        .periph_ppc_mask = AC_UART1,
};

struct platform_data_t
    tfm_peripheral_FF_TEST_WATCHDOG_REGION = {
        .periph_start = WATCHDOG_BASE,
        .periph_limit = WATCHDOG_BASE + 0x3FFF,
        .periph_ppc_mask = AC_DO_NOT_CONFIGURE,
};

struct platform_data_t
    tfm_peripheral_FF_TEST_NVMEM_REGION = {
        .periph_start = FF_TEST_NVMEM_REGION_START,
        .periph_limit = FF_TEST_NVMEM_REGION_END,
        .periph_ppc_mask = AC_DO_NOT_CONFIGURE
};

struct platform_data_t
    tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO = {
        .periph_start = FF_TEST_SERVER_PARTITION_MMIO_START,
        .periph_limit = FF_TEST_SERVER_PARTITION_MMIO_END,
        .periph_ppc_mask = AC_DO_NOT_CONFIGURE
};

struct platform_data_t
    tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO = {
        .periph_start = FF_TEST_DRIVER_PARTITION_MMIO_START,
        .periph_limit = FF_TEST_DRIVER_PARTITION_MMIO_END,
        .periph_ppc_mask = AC_DO_NOT_CONFIGURE
};
#endif

/*------------------- SAU/IDAU configuration functions -----------------------*/
void sau_and_idau_cfg(void)
{
    /* Ensure all memory accesses are completed */
    __DMB();
    #if 0 /* Bootrom set to be secure temporary */
    /* Configures SAU regions to be non-secure */
    /* Configure Bootrom */
    SAU->RNR = 0;
    SAU->RBAR = (ROM_BASE & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = ((ROM_BASE + 0x7E00 - 1) & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk;

    /* Configure Bootrom SGs */
    SAU->RNR = 1;
    SAU->RBAR = ((ROM_BASE + 0x7E00) & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = ((ROM_BASE + 0x7FFF) & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk | SAU_RLAR_NSC_Msk;
    #endif

    /* Configures veneers region to be non-secure callable */
    SAU->RNR  = 2;
    SAU->RBAR = (memory_regions.veneer_base & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (memory_regions.veneer_limit & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk | SAU_RLAR_NSC_Msk;

    /* Configure Non-Secure partition in flash */
    SAU->RNR = 3;
    SAU->RBAR = (memory_regions.non_secure_partition_base
                 & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (memory_regions.non_secure_partition_limit
                  & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

    /* Configure the rest of the address map up to PPB */
    SAU->RNR = 4;
    SAU->RBAR = (SRAM4_BASE & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = ((PPB_BASE - 1) & SAU_RLAR_LADDR_Msk)
                | SAU_RLAR_ENABLE_Msk;

    /* Turn off unused bootrom region */
    SAU->RNR = 7;
    SAU->RBAR = 0;
    SAU->RLAR = 0;

    /* Enables SAU */
    TZ_SAU_Enable();
    /* Add barriers to assure the SAU configuration is done before continue
     * the execution.
     */
    __DSB();
    __ISB();
}

enum tfm_plat_err_t bus_filter_cfg(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint32_t c0_c1_unpriv_periph_offsets[] = {AC_SRAM0,
                AC_SRAM1, AC_SRAM2, AC_SRAM3};
    uint8_t nbr_of_c0_c1_unpriv_periphs;
    uint32_t c0_unpriv_periph_offsets[] = {AC_SYSCFG, AC_CLOCKS_BANK_DEFAULT,
                AC_RSM, AC_BUSCTRL, AC_OTP, AC_POWMAN, AC_TRNG, AC_XOSC,
                AC_ROSC, AC_PLL_SYS, AC_PLL_USB, AC_TICKS, AC_XIP_CTRL,
                AC_XIP_QMI};
    uint8_t nbr_of_c0_unpriv_periphs;
    uint32_t ns_priv_periph_offsets[] = {AC_ROM, AC_XIP_MAIN, AC_SRAM4,
                AC_SRAM5, AC_SRAM6, AC_SRAM7, AC_SRAM8, AC_SRAM9, AC_DMA,
                AC_USBCTRL, AC_PIO0, AC_PIO1, AC_PIO2, AC_CORESIGHT_TRACE,
                AC_CORESIGHT_PERIPH, AC_SYSINFO, AC_RESETS, AC_IO_BANK0,
                AC_IO_BANK1, AC_PADS_BANK0, AC_PADS_QSPI, AC_ADC0, AC_HSTX,
                AC_I2C0, AC_I2C1, AC_PWM, AC_SPI0, AC_SPI1, AC_TIMER0,
                AC_TIMER1, AC_UART0, AC_UART1, AC_TBMAN, AC_SHA256, AC_WATCHDOG,
                AC_XIP_AUX};
    uint8_t nbr_of_ns_periphs;
    uint32_t temp_addr;

    nbr_of_c0_c1_unpriv_periphs = sizeof(c0_c1_unpriv_periph_offsets) /
        sizeof(c0_c1_unpriv_periph_offsets[0]);
    nbr_of_c0_unpriv_periphs = sizeof(c0_unpriv_periph_offsets) /
        sizeof(c0_unpriv_periph_offsets[0]);
    nbr_of_ns_periphs = sizeof(ns_priv_periph_offsets) /
        sizeof(ns_priv_periph_offsets[0]);

    /* Probably worth doing a software reset of access ctrl before setup */
    accessctrl_hw->cfgreset = ACCESSCTRL_PASSWORD_BITS  | 0x1;

    accessctrl_hw->gpio_nsmask[0] = 0xFFFFFFFC;
    accessctrl_hw->gpio_nsmask[1] = 0xFF00FFFF;

    /* Peripherals controlled by Secure Core0 and Core1 */
    for (uint8_t i = 0; i < nbr_of_c0_c1_unpriv_periphs; i++){
        temp_addr = ACCESSCTRL_BASE + c0_c1_unpriv_periph_offsets[i];
         REG_RW(temp_addr) = ACCESSCTRL_S_UNPRIV_C0_C1 |
            ACCESSCTRL_PASSWORD_BITS;
        if (REG_RW(temp_addr) != ACCESSCTRL_S_UNPRIV_C0_C1) {
            err = TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    /* Peripherals controlled by Secure Core0 */
    for (uint8_t i = 0; i < nbr_of_c0_unpriv_periphs; i++){
        temp_addr = ACCESSCTRL_BASE + c0_unpriv_periph_offsets[i];
         REG_RW(temp_addr) = ACCESSCTRL_S_UNPRIV_C0 |
            ACCESSCTRL_PASSWORD_BITS;
        if (REG_RW(temp_addr) != ACCESSCTRL_S_UNPRIV_C0) {
            err = TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    /* Peripherals accessable to all bus actors */
    for (uint8_t i = 0; i < nbr_of_ns_periphs; i++){
        temp_addr = ACCESSCTRL_BASE + ns_priv_periph_offsets[i];
         REG_RW(temp_addr) = ACCESSCTRL_NS_PRIV |
            ACCESSCTRL_PASSWORD_BITS;
        if (REG_RW(temp_addr) != ACCESSCTRL_NS_PRIV) {
            err = TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    /* Lock setings for every actor except Core0, only hard reset can clear
       this. Core0 must retain control for mmio control. */
    accessctrl_hw->lock = ACCESSCTRL_PASSWORD_BITS  | 0x6;

    return err;
}

void access_ctrl_configure_to_secure_privileged(access_ctrl_reg_offset offset)
{
    if (offset != AC_DO_NOT_CONFIGURE){
        REG_RW(ACCESSCTRL_BASE + offset) =
            ACCESSCTRL_S_PRIV_C0 | ACCESSCTRL_PASSWORD_BITS;
    }
}

void access_ctrl_configure_to_secure_unprivileged(access_ctrl_reg_offset offset)
{
    if (offset != AC_DO_NOT_CONFIGURE){
        REG_RW(ACCESSCTRL_BASE + offset) =
            ACCESSCTRL_S_UNPRIV_C0 | ACCESSCTRL_PASSWORD_BITS;
    }
}

enum tfm_plat_err_t dma_security_config(void)
{
    /* Configure every DMA channel as Nonsecure Privileged since TF-M uses no DMA */
    for (int i=0; i<16; i++){
        REG_RW(DMA_BASE + DMA_SECCFG_CH0_OFFSET + (i * 4)) =
            DMA_SECCFG_CH0_P_BITS;
    }

    /* Configure DMA MPU to mirror SAU settings */
    /* Unmapped address regions default to SP */
    dma_hw->mpu_ctrl = DMA_MPU_CTRL_NS_HIDE_ADDR_BITS | DMA_MPU_CTRL_S_BITS |
                       DMA_MPU_CTRL_P_BITS;

    /* Configure MPU regions */
    dma_hw->mpu_region[0].bar = (memory_regions.veneer_base &
                                 DMA_MPU_BAR0_BITS);
    dma_hw->mpu_region[0].lar = (memory_regions.veneer_limit &
                                 DMA_MPU_LAR0_ADDR_BITS) |
                                DMA_MPU_LAR0_P_BITS | DMA_MPU_LAR0_EN_BITS;

    dma_hw->mpu_region[1].bar = (memory_regions.non_secure_partition_base &
                                 DMA_MPU_BAR0_BITS);
    dma_hw->mpu_region[1].lar = (memory_regions.veneer_limit &
                                 DMA_MPU_LAR0_ADDR_BITS) |
                                DMA_MPU_LAR0_P_BITS | DMA_MPU_LAR0_EN_BITS;

    dma_hw->mpu_region[2].bar = (SRAM4_BASE & DMA_MPU_BAR0_BITS);
    dma_hw->mpu_region[2].lar = ((PPB_BASE - 1) & DMA_MPU_LAR0_ADDR_BITS) |
                                DMA_MPU_LAR0_P_BITS | DMA_MPU_LAR0_EN_BITS;

    dma_hw->mpu_region[3].bar = 0;
    dma_hw->mpu_region[3].lar = 0;

    dma_hw->mpu_region[4].bar = 0;
    dma_hw->mpu_region[4].lar = 0;

    dma_hw->mpu_region[5].bar = 0;
    dma_hw->mpu_region[5].lar = 0;

    dma_hw->mpu_region[6].bar = 0;
    dma_hw->mpu_region[6].lar = 0;

    dma_hw->mpu_region[7].bar = 0;
    dma_hw->mpu_region[7].lar = 0;

    return TFM_PLAT_ERR_SUCCESS;
}

