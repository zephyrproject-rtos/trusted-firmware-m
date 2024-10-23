/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_hal_device_header.h"
#include "Driver_Flash_RPI.h"
#include "RTE_Device.h"

#include "armv8m_mpu.h"

#ifdef TFM_MULTI_CORE_TOPOLOGY
#include "platform_multicore.h"
#include "hardware/structs/sio.h"
#endif

#if (RTE_FLASH0)

#define RP2350_FLASH_PAGE_SIZE        0x100        /* 256B */
#define RP2350_FLASH_SECTOR_SIZE      0x1000       /* 4KB */
#define RP2350_FLASH_SIZE             PICO_FLASH_SIZE_BYTES
#define RP2350_FLASH_ERASE_VALUE      0xFF

static ARM_FLASH_INFO RP2350_FLASH_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = RP2350_FLASH_SIZE/ RP2350_FLASH_SECTOR_SIZE,
    .sector_size    = RP2350_FLASH_SECTOR_SIZE,
    .page_size      = RP2350_FLASH_PAGE_SIZE,
    .program_unit   = RP2350_FLASH_PAGE_SIZE, /* page aligned, page multipled */
    .erased_value   = RP2350_FLASH_ERASE_VALUE
};

#define MPU_REGION_NUMBER   8
#define SEC_STATE_NUM 2

struct mpu_state_save {
    uint32_t mpu;
    uint32_t shcsr;
    uint32_t mair[2];
    ARM_MPU_Region_t mpu_table[MPU_REGION_NUMBER];
};

static struct mpu_state_save mpu_state[SEC_STATE_NUM];
static uint32_t irq_state = 0;
MPU_Type* mpu_p[SEC_STATE_NUM] = {MPU, MPU_NS};
SCB_Type* scb_p[SEC_STATE_NUM] = {SCB, SCB_NS};

static inline uint32_t __save_disable_irq(void)
{
    uint32_t result = 0;

    /* Claim lock of Flash */
#ifdef TFM_MULTI_CORE_TOPOLOGY
    while(!*FLASH_SPINLOCK);
#endif
    __ASM volatile ("mrs %0, primask \n cpsid i" : "=r" (result) :: "memory");
#ifdef TFM_MULTI_CORE_TOPOLOGY
    /* Signal Core1 to wait for flash */
    sio_hw->doorbell_out_set = FLASH_DOORBELL_MASK;
    if (CORE1_RUNNING)
    {
        /* Wait for Core1 to clear doorbell */
        while(sio_hw->doorbell_out_set & FLASH_DOORBELL_MASK);
    }
#endif
    return result;
}

static inline void __restore_irq(uint32_t status)
{
    __ASM volatile ("msr primask, %0" :: "r" (status) : "memory");
    /* Release lock of Flash */
#ifdef TFM_MULTI_CORE_TOPOLOGY
    *FLASH_SPINLOCK = 0x1;
#endif
}

/* This function must be placed in RAM, so when MPU configuration is saved and
   flash is protected by a non-executable region MemManageFault is avoided.
   Since PRIVDEFENA is set the system memory map is enabled for privileged code
   and execution from RAM is available */
static void __not_in_flash_func(mpu_state_save)
                                            (struct rp2350_flash_dev_t* flash_dev)
{
    static const uint8_t mpu_attr_num = 0;
    uint32_t memory_base = flash_dev->base;
    uint32_t memory_limit = flash_dev->base + flash_dev->size -1;

    irq_state = __save_disable_irq();

    for(int i=0; i<SEC_STATE_NUM; i++) {

    mpu_state[i].shcsr = scb_p[i]->SHCSR;
    mpu_state[i].mpu = mpu_p[i]->CTRL;

        if(mpu_p[i] == MPU) {
            ARM_MPU_Disable();
        } else {
            ARM_MPU_Disable_NS();
        }

        for(uint8_t j = 0; j < MPU_REGION_NUMBER; j++) {
            mpu_p[i]->RNR = j;
            mpu_state[i].mpu_table[j].RBAR = mpu_p[i]->RBAR;
            mpu_state[i].mpu_table[j].RLAR = mpu_p[i]->RLAR;
            mpu_p[i]->RBAR = 0;
            mpu_p[i]->RLAR = 0;
        }

        mpu_state[i].mair[0] = mpu_p[i]->MAIR[0];
        mpu_state[i].mair[1] = mpu_p[i]->MAIR[1];

        mpu_p[i]->MAIR[0] = 0;
        mpu_p[i]->MAIR[1] = 0;

        /* Attr0 : Device memory, nGnRE */
        if(mpu_p[i] == MPU) {
            ARM_MPU_SetMemAttr(mpu_attr_num,
                               ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE,
                                            ARM_MPU_ATTR_DEVICE_nGnRE));
        } else {
            ARM_MPU_SetMemAttr_NS(mpu_attr_num,
                                  ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE,
                                               ARM_MPU_ATTR_DEVICE_nGnRE));
        }

        mpu_p[i]->RNR = 0;
        mpu_p[i]->RBAR = ARM_MPU_RBAR(memory_base,
                                 ARM_MPU_SH_NON,
                                 1,
                                 0,
                                 1);
        #ifdef TFM_PXN_ENABLE
        mpu_p[i]->RLAR = ARM_MPU_RLAR_PXN(memory_limit, 1, mpu_attr_num);
        #else
        mpu_p[i]->RLAR = ARM_MPU_RLAR(memory_limit, mpu_attr_num);
        #endif

        if(mpu_p[i] == MPU) {
            ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
        } else {
            ARM_MPU_Enable_NS(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
        }
    }
}

static void __not_in_flash_func(mpu_state_restore)(void)
{
    for(int i=0; i<SEC_STATE_NUM; i++) {

        if(mpu_p[i] == MPU) {
            ARM_MPU_Disable();
        } else {
            ARM_MPU_Disable_NS();
        }

        for(uint8_t j = 0; j < MPU_REGION_NUMBER; j++) {
            mpu_p[i]->RNR = j;
            mpu_p[i]->RBAR = mpu_state[i].mpu_table[j].RBAR;
            mpu_p[i]->RLAR = mpu_state[i].mpu_table[j].RLAR;
        }

        mpu_p[i]->MAIR[0] = mpu_state[i].mair[0];
        mpu_p[i]->MAIR[1] = mpu_state[i].mair[1];

        __DMB();
        mpu_p[i]->CTRL = mpu_state[i].mpu;
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
        scb_p[i]->SHCSR = mpu_state[i].shcsr;
#endif
        __DSB();
        __ISB();

    }

    __restore_irq(irq_state);
}

static rp2350_flash_dev_t RP2350_FLASH_DEV = {
    .data = &RP2350_FLASH_DEV_DATA,
    .base = XIP_BASE,
    .size = RP2350_FLASH_SIZE,
    .save_mpu_state = mpu_state_save,
    .restore_mpu_state = mpu_state_restore
};


RPI_RP2350_FLASH(RP2350_FLASH_DEV, RP2350_FLASH);
#endif /* RTE_FLASH0 */
