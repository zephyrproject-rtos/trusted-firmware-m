/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "region.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "bootutil/fault_injection_hardening.h"
#include "firewall.h"

#if defined(CRYPTO_HW_ACCELERATOR) || \
    defined(CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING)
#include "crypto_hw.h"
#endif

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

extern ARM_DRIVER_FLASH FLASH_DEV_NAME_SE_SECURE_FLASH;

REGION_DECLARE(Image$$, ER_DATA, $$Base)[];
REGION_DECLARE(Image$$, ARM_LIB_HEAP, $$ZI$$Limit)[];

#define HOST_BIR_BASE                       0x00000000
#define HOST_SHARED_RAM_BASE                0x02000000
#define HOST_XNVM_BASE                      0x08000000
#define AXI_QSPI_CTRL_REG_BASE              0x40050000
#define HOST_BASE_SYSTEM_CONTROL_BASE       0x1A010000
#define HOST_FIREWALL_BASE                  0x1A800000
#define HOST_FPGA_SCC_REGISTERS             0x40000000
#define HOST_SE_SECURE_FLASH_BASE_FVP       0x60010000
#define FW_CONTROLLER                       0
#define COMP_FC1                            1
#define SE_MID                              0

static void setup_se_firewall(void)
{
    enum rgn_mpl_t mpl_rights = 0;

#if !(PLATFORM_IS_FVP)
    /* Configure the SE firewall controller */
    fc_select((void *)CORSTONE1000_FIREWALL_BASE, FW_CONTROLLER);
    fc_disable_bypass();
    fc_select_region(2);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_2MB, CORSTONE1000_FIREWALL_BASE);
    fc_init_mpl(RGN_MPE0);
    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);
    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();
    fc_pe_enable();
#endif

    /* Configure the SE firewall component 1 */
    fc_select((void *)CORSTONE1000_FIREWALL_BASE, COMP_FC1);
    fc_disable_bypass();
    fc_pe_disable();

    /* Boot Instruction Register region: 4KB */
    fc_select_region(1);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_4KB, CORSTONE1000_HOST_BIR_BASE);
    fc_prog_rgn_upper_addr(HOST_BIR_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

    /* Shared RAM region: 4MB */
    fc_select_region(2);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_4MB, CORSTONE1000_HOST_SHARED_RAM_BASE);
    fc_prog_rgn_upper_addr(HOST_SHARED_RAM_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK |
                  RGN_MPL_SECURE_EXECUTE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

    /* XNVM: 32MB */
    fc_select_region(3);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_32MB, CORSTONE1000_HOST_XNVM_BASE);
    fc_prog_rgn_upper_addr(HOST_XNVM_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);
    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

    /* Host SCB Registers: 64KB */
    fc_select_region(4);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_64KB, CORSTONE1000_HOST_BASE_SYSTEM_CONTROL_BASE);
    fc_prog_rgn_upper_addr(HOST_BASE_SYSTEM_CONTROL_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

    /* Host firewall: 2MB */
    fc_select_region(5);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_2MB, CORSTONE1000_HOST_FIREWALL_BASE);
    fc_prog_rgn_upper_addr(HOST_FIREWALL_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);
    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

#if PLATFORM_IS_FVP
    /* SE Flash Write: 8MB */
    fc_select_region(6);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_8MB, CORSTONE1000_SE_SECURE_FLASH_BASE_FVP);
    fc_prog_rgn_upper_addr(HOST_SE_SECURE_FLASH_BASE_FVP);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();
#else
    /* QSPI Flash Write: 64KB */
    fc_select_region(6);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_64KB, CORSTONE1000_AXI_QSPI_CTRL_REG_BASE);
    fc_prog_rgn_upper_addr(AXI_QSPI_CTRL_REG_BASE);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK);

    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();

    /* FPGA – SCC Registers: 64KB */
    fc_select_region(7);
    fc_disable_regions();
    fc_disable_mpe(RGN_MPE0);
    fc_prog_rgn(RGN_SIZE_4KB, CORSTONE1000_HOST_FPGA_SCC_REGISTERS);
    fc_prog_rgn_upper_addr(HOST_FPGA_SCC_REGISTERS);
    fc_enable_addr_trans();
    fc_init_mpl(RGN_MPE0);

    mpl_rights = (RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK |
                  RGN_MPL_SECURE_EXECUTE_MASK |
                  RGN_MPL_NONSECURE_READ_MASK |
                  RGN_MPL_NONSECURE_WRITE_MASK |
                  RGN_MPL_NONSECURE_EXECUTE_MASK);

    /* Enable All accesses from boot-processor */
    fc_enable_mpl(RGN_MPE0, mpl_rights);
    fc_prog_mid(RGN_MPE0, SE_MID);
    fc_enable_mpe(RGN_MPE0);
    fc_enable_regions();
#endif

    fc_pe_enable();
}

__attribute__((naked)) void boot_clear_bl2_ram_area(void)
{
    __ASM volatile(
        ".syntax unified                             \n"
        "movs    r0, #0                              \n"
        "ldr     r1, =Image$$ER_DATA$$Base           \n"
        "ldr     r2, =Image$$ARM_LIB_HEAP$$ZI$$Limit \n"
        "subs    r2, r2, r1                          \n"
        "Loop:                                       \n"
        "subs    r2, #4                              \n"
        "blt     Clear_done                          \n"
        "str     r0, [r1, r2]                        \n"
        "b       Loop                                \n"
        "Clear_done:                                 \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}

int32_t boot_platform_init(void)
{
    int32_t result;

    setup_se_firewall();

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#if PLATFORM_IS_FVP
   result = FLASH_DEV_NAME_SE_SECURE_FLASH.Initialize(NULL);
   if (result != ARM_DRIVER_OK) {
       return 1;
   }
#endif

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    return 0;
}

void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;
    int32_t result;

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while (1);
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1);
    }

#if PLATFORM_IS_FVP
    result = FLASH_DEV_NAME_SE_SECURE_FLASH.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1);
    }
#endif

    vt_cpy = vt;

    __set_MSP(vt->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
