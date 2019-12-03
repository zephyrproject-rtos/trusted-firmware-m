/*
 * Copyright (c) 2018 Arm Limited
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
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

#include <assert.h>
#include <stdio.h> /* for debugging printfs */
#include "cy_prot.h"
#include "cycfg.h"
#include "device_definition.h"
#include "driver_ppu.h"
#include "driver_smpu.h"
#include "pc_config.h"
#include "platform_description.h"
#include "region_defs.h"
#include "RTE_Device.h"
#include "target_cfg.h"
#include "tfm_plat_defs.h"


/* Macros to pick linker symbols */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

const struct memory_region_limits memory_regions = {
    .non_secure_code_start =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        BL2_HEADER_SIZE,

    .non_secure_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

    .non_secure_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1,
};


#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
        SCB5_BASE,
        SCB5_BASE + 0xFFF,
        -1,
        -1
};

void enable_fault_handlers(void)
{
    /* Fault handles enable registers are not present in Cortex-M0+ */
}

void system_reset_cfg(void)
{
    uint32_t reg_value = SCB->AIRCR;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Set Access Key (0x05FA must be written to this field) */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK);

    SCB->AIRCR = reg_value;
}

extern void Cy_Platform_Init(void);
void platform_init(void)
{
    Cy_PDL_Init(CY_DEVICE_CFG);

    init_cycfg_all();
    Cy_Platform_Init();

    /* make sure CM4 is disabled */
    if (CY_SYS_CM4_STATUS_ENABLED == Cy_SysGetCM4Status()) {
        Cy_SysDisableCM4();
    }
}

enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    printf("%s()\n", __func__);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t nvic_interrupt_enable(void)
{
    /* PPU and SMPU don't generate interrupts.
     * USART and Flash drivers don't export an EnableInterrupt function.
     * So there's nothing to do here.
     */
    return TFM_PLAT_ERR_SUCCESS;
}

static cy_en_prot_status_t set_bus_master_attr(void)
{
    cy_en_prot_status_t ret;

    printf("%s()\n", __func__);

    /* Cortex-M4 - PC=6 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_CM4, CY_PROT_HOST_DEFAULT);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    /* Test Controller - PC=7 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_TC, CY_PROT_TC);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    /* Cortex-M0+ - PC=1 */
    ret = Cy_Prot_SetActivePC(CPUSS_MS_ID_CM0, CY_PROT_SPM_DEFAULT);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }

    return CY_PROT_SUCCESS;
}

void bus_masters_cfg(void)
{
    cy_en_prot_status_t ret = set_bus_master_attr();
    if (ret) {
        printf("set_bus_master_attr() returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
}

void smpu_init_cfg(void)
{
    cy_en_prot_status_t ret;

    printf("%s()\n", __func__);

    printf("memory_regions.non_secure_code_start = %#x\n", memory_regions.non_secure_code_start);
    printf("memory_regions.non_secure_partition_base = %#x\n", memory_regions.non_secure_partition_base);
    printf("memory_regions.non_secure_partition_limit = %#x\n", memory_regions.non_secure_partition_limit);

#if RTE_SMPU13
    ret = SMPU_Configure(&SMPU13_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU13) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU12
    ret = SMPU_Configure(&SMPU12_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU12) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU11
    ret = SMPU_Configure(&SMPU11_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU11) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU10
    ret = SMPU_Configure(&SMPU10_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU10) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU9
    ret = SMPU_Configure(&SMPU9_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU9) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU8
    ret = SMPU_Configure(&SMPU8_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU8) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU7
    ret = SMPU_Configure(&SMPU7_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU7) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU6
    ret = SMPU_Configure(&SMPU6_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU6) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU5
    ret = SMPU_Configure(&SMPU5_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU5) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU4
    ret = SMPU_Configure(&SMPU4_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU4) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU3
    ret = SMPU_Configure(&SMPU3_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU3) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU2
    ret = SMPU_Configure(&SMPU2_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU2) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU1
    ret = SMPU_Configure(&SMPU1_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU1) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif
#if RTE_SMPU0
    ret = SMPU_Configure(&SMPU0_Resources);
    if (ret) {
        printf("SMPU_Configure(SMPU0) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

    /* Now protect all unconfigured SMPUs */
    ret = protect_unconfigured_smpus();
    if (ret) {
        printf("protect_unconfigured_smpus() returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);

    __DSB();
    __ISB();
}

void ppu_init_cfg(void)
{
    cy_en_prot_status_t ret;
    (void)ret;

    printf("%s()\n", __func__);

#if RTE_MS_PPU_PR7
    ret = PPU_Configure(&PR7_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PR7_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_MAIN
    ret = PPU_Configure(&PERI_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR0_GROUP
    ret = PPU_Configure(&PERI_GR0_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR0_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR1_GROUP
    ret = PPU_Configure(&PERI_GR1_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR1_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR2_GROUP
    ret = PPU_Configure(&PERI_GR2_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR2_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR3_GROUP
    ret = PPU_Configure(&PERI_GR3_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR3_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR4_GROUP
    ret = PPU_Configure(&PERI_GR4_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR4_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR6_GROUP
    ret = PPU_Configure(&PERI_GR6_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR6_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR9_GROUP
    ret = PPU_Configure(&PERI_GR9_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR9_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_GR10_GROUP
    ret = PPU_Configure(&PERI_GR10_GROUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_GR10_GROUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PERI_TR
    ret = PPU_Configure(&PERI_TR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PERI_TR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_MAIN
    ret = PPU_Configure(&CRYPTO_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_CRYPTO
    ret = PPU_Configure(&CRYPTO_CRYPTO_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_CRYPTO_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_BOOT
    ret = PPU_Configure(&CRYPTO_BOOT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_BOOT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_KEY0
    ret = PPU_Configure(&CRYPTO_KEY0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_KEY0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_KEY1
    ret = PPU_Configure(&CRYPTO_KEY1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_KEY1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CRYPTO_BUF
    ret = PPU_Configure(&CRYPTO_BUF_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CRYPTO_BUF_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CPUSS_CM4
    ret = PPU_Configure(&CPUSS_CM4_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CPUSS_CM4_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CPUSS_CM0
    ret = PPU_Configure(&CPUSS_CM0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CPUSS_CM0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CPUSS_BOOT
    ret = PPU_Configure(&CPUSS_BOOT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CPUSS_BOOT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CPUSS_CM0_INT
    ret = PPU_Configure(&CPUSS_CM0_INT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CPUSS_CM0_INT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CPUSS_CM4_INT
    ret = PPU_Configure(&CPUSS_CM4_INT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CPUSS_CM4_INT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FAULT_STRUCT0_MAIN
    ret = PPU_Configure(&FAULT_STRUCT0_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FAULT_STRUCT0_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FAULT_STRUCT1_MAIN
    ret = PPU_Configure(&FAULT_STRUCT1_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FAULT_STRUCT1_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT0_IPC
    ret = PPU_Configure(&IPC_STRUCT0_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT0_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT1_IPC
    ret = PPU_Configure(&IPC_STRUCT1_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT1_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT2_IPC
    ret = PPU_Configure(&IPC_STRUCT2_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT2_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT3_IPC
    ret = PPU_Configure(&IPC_STRUCT3_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT3_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT4_IPC
    ret = PPU_Configure(&IPC_STRUCT4_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT4_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT5_IPC
    ret = PPU_Configure(&IPC_STRUCT5_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT5_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT6_IPC
    ret = PPU_Configure(&IPC_STRUCT6_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT6_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT7_IPC
    ret = PPU_Configure(&IPC_STRUCT7_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT7_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT8_IPC
    ret = PPU_Configure(&IPC_STRUCT8_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT8_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT9_IPC
    ret = PPU_Configure(&IPC_STRUCT9_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT9_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT10_IPC
    ret = PPU_Configure(&IPC_STRUCT10_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT10_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT11_IPC
    ret = PPU_Configure(&IPC_STRUCT11_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT11_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT12_IPC
    ret = PPU_Configure(&IPC_STRUCT12_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT12_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT13_IPC
    ret = PPU_Configure(&IPC_STRUCT13_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT13_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT14_IPC
    ret = PPU_Configure(&IPC_STRUCT14_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT14_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_STRUCT15_IPC
    ret = PPU_Configure(&IPC_STRUCT15_IPC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_STRUCT15_IPC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT0_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT0_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT0_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT1_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT1_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT1_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT2_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT2_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT2_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT3_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT3_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT3_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT4_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT4_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT4_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT5_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT5_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT5_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT6_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT6_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT6_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT7_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT7_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT7_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT8_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT8_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT8_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT9_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT9_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT9_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT10_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT10_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT10_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT11_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT11_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT11_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT12_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT12_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT12_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT13_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT13_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT13_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT14_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT14_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT14_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_IPC_INTR_STRUCT15_INTR
    ret = PPU_Configure(&IPC_INTR_STRUCT15_INTR_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&IPC_INTR_STRUCT15_INTR_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_SMPU_MAIN
    ret = PPU_Configure(&PROT_SMPU_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_SMPU_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_MPU0_MAIN
    ret = PPU_Configure(&PROT_MPU0_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_MPU0_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_MPU5_MAIN
    ret = PPU_Configure(&PROT_MPU5_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_MPU5_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_MPU6_MAIN
    ret = PPU_Configure(&PROT_MPU6_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_MPU6_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_MPU14_MAIN
    ret = PPU_Configure(&PROT_MPU14_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_MPU14_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROT_MPU15_MAIN
    ret = PPU_Configure(&PROT_MPU15_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROT_MPU15_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_MAIN
    ret = PPU_Configure(&FLASHC_MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_CMD
    ret = PPU_Configure(&FLASHC_CMD_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_CMD_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_DFT
    ret = PPU_Configure(&FLASHC_DFT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_DFT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_CM0
    ret = PPU_Configure(&FLASHC_CM0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_CM0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_CM4
    ret = PPU_Configure(&FLASHC_CM4_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_CM4_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_CRYPTO
    ret = PPU_Configure(&FLASHC_CRYPTO_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_CRYPTO_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_DW0
    ret = PPU_Configure(&FLASHC_DW0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_DW0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_DW1
    ret = PPU_Configure(&FLASHC_DW1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_DW1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_DMAC
    ret = PPU_Configure(&FLASHC_DMAC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_DMAC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_EXT_MS0
    ret = PPU_Configure(&FLASHC_EXT_MS0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_EXT_MS0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_EXT_MS1
    ret = PPU_Configure(&FLASHC_EXT_MS1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_EXT_MS1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_FLASHC_FM
    ret = PPU_Configure(&FLASHC_FM_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&FLASHC_FM_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN1
    ret = PPU_Configure(&SRSS_MAIN1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN2
    ret = PPU_Configure(&SRSS_MAIN2_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN2_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_WDT
    ret = PPU_Configure(&WDT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&WDT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_MAIN
    ret = PPU_Configure(&MAIN_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&MAIN_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN3
    ret = PPU_Configure(&SRSS_MAIN3_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN3_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN4
    ret = PPU_Configure(&SRSS_MAIN4_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN4_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN5
    ret = PPU_Configure(&SRSS_MAIN5_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN5_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN6
    ret = PPU_Configure(&SRSS_MAIN6_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN6_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SRSS_MAIN7
    ret = PPU_Configure(&SRSS_MAIN7_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SRSS_MAIN7_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_BACKUP_BACKUP
    ret = PPU_Configure(&BACKUP_BACKUP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&BACKUP_BACKUP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_DW
    ret = PPU_Configure(&DW0_DW_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_DW_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_DW
    ret = PPU_Configure(&DW1_DW_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_DW_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_DW_CRC
    ret = PPU_Configure(&DW0_DW_CRC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_DW_CRC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_DW_CRC
    ret = PPU_Configure(&DW1_DW_CRC_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_DW_CRC_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT0_CH
    ret = PPU_Configure(&DW0_CH_STRUCT0_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT0_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT1_CH
    ret = PPU_Configure(&DW0_CH_STRUCT1_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT1_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT2_CH
    ret = PPU_Configure(&DW0_CH_STRUCT2_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT2_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT3_CH
    ret = PPU_Configure(&DW0_CH_STRUCT3_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT3_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT4_CH
    ret = PPU_Configure(&DW0_CH_STRUCT4_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT4_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT5_CH
    ret = PPU_Configure(&DW0_CH_STRUCT5_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT5_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT6_CH
    ret = PPU_Configure(&DW0_CH_STRUCT6_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT6_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT7_CH
    ret = PPU_Configure(&DW0_CH_STRUCT7_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT7_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT8_CH
    ret = PPU_Configure(&DW0_CH_STRUCT8_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT8_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT9_CH
    ret = PPU_Configure(&DW0_CH_STRUCT9_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT9_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT10_CH
    ret = PPU_Configure(&DW0_CH_STRUCT10_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT10_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT11_CH
    ret = PPU_Configure(&DW0_CH_STRUCT11_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT11_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT12_CH
    ret = PPU_Configure(&DW0_CH_STRUCT12_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT12_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT13_CH
    ret = PPU_Configure(&DW0_CH_STRUCT13_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT13_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT14_CH
    ret = PPU_Configure(&DW0_CH_STRUCT14_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT14_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT15_CH
    ret = PPU_Configure(&DW0_CH_STRUCT15_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT15_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT16_CH
    ret = PPU_Configure(&DW0_CH_STRUCT16_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT16_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT17_CH
    ret = PPU_Configure(&DW0_CH_STRUCT17_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT17_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT18_CH
    ret = PPU_Configure(&DW0_CH_STRUCT18_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT18_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT19_CH
    ret = PPU_Configure(&DW0_CH_STRUCT19_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT19_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT20_CH
    ret = PPU_Configure(&DW0_CH_STRUCT20_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT20_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT21_CH
    ret = PPU_Configure(&DW0_CH_STRUCT21_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT21_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT22_CH
    ret = PPU_Configure(&DW0_CH_STRUCT22_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT22_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT23_CH
    ret = PPU_Configure(&DW0_CH_STRUCT23_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT23_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT24_CH
    ret = PPU_Configure(&DW0_CH_STRUCT24_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT24_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT25_CH
    ret = PPU_Configure(&DW0_CH_STRUCT25_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT25_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT26_CH
    ret = PPU_Configure(&DW0_CH_STRUCT26_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT26_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT27_CH
    ret = PPU_Configure(&DW0_CH_STRUCT27_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT27_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW0_CH_STRUCT28_CH
    ret = PPU_Configure(&DW0_CH_STRUCT28_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW0_CH_STRUCT28_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT0_CH
    ret = PPU_Configure(&DW1_CH_STRUCT0_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT0_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT1_CH
    ret = PPU_Configure(&DW1_CH_STRUCT1_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT1_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT2_CH
    ret = PPU_Configure(&DW1_CH_STRUCT2_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT2_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT3_CH
    ret = PPU_Configure(&DW1_CH_STRUCT3_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT3_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT4_CH
    ret = PPU_Configure(&DW1_CH_STRUCT4_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT4_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT5_CH
    ret = PPU_Configure(&DW1_CH_STRUCT5_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT5_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT6_CH
    ret = PPU_Configure(&DW1_CH_STRUCT6_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT6_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT7_CH
    ret = PPU_Configure(&DW1_CH_STRUCT7_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT7_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT8_CH
    ret = PPU_Configure(&DW1_CH_STRUCT8_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT8_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT9_CH
    ret = PPU_Configure(&DW1_CH_STRUCT9_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT9_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT10_CH
    ret = PPU_Configure(&DW1_CH_STRUCT10_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT10_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT11_CH
    ret = PPU_Configure(&DW1_CH_STRUCT11_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT11_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT12_CH
    ret = PPU_Configure(&DW1_CH_STRUCT12_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT12_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT13_CH
    ret = PPU_Configure(&DW1_CH_STRUCT13_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT13_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT14_CH
    ret = PPU_Configure(&DW1_CH_STRUCT14_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT14_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT15_CH
    ret = PPU_Configure(&DW1_CH_STRUCT15_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT15_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT16_CH
    ret = PPU_Configure(&DW1_CH_STRUCT16_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT16_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT17_CH
    ret = PPU_Configure(&DW1_CH_STRUCT17_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT17_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT18_CH
    ret = PPU_Configure(&DW1_CH_STRUCT18_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT18_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT19_CH
    ret = PPU_Configure(&DW1_CH_STRUCT19_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT19_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT20_CH
    ret = PPU_Configure(&DW1_CH_STRUCT20_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT20_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT21_CH
    ret = PPU_Configure(&DW1_CH_STRUCT21_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT21_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT22_CH
    ret = PPU_Configure(&DW1_CH_STRUCT22_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT22_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT23_CH
    ret = PPU_Configure(&DW1_CH_STRUCT23_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT23_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT24_CH
    ret = PPU_Configure(&DW1_CH_STRUCT24_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT24_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT25_CH
    ret = PPU_Configure(&DW1_CH_STRUCT25_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT25_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT26_CH
    ret = PPU_Configure(&DW1_CH_STRUCT26_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT26_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT27_CH
    ret = PPU_Configure(&DW1_CH_STRUCT27_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT27_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DW1_CH_STRUCT28_CH
    ret = PPU_Configure(&DW1_CH_STRUCT28_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DW1_CH_STRUCT28_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DMAC_TOP
    ret = PPU_Configure(&DMAC_TOP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DMAC_TOP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DMAC_CH0_CH
    ret = PPU_Configure(&DMAC_CH0_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DMAC_CH0_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DMAC_CH1_CH
    ret = PPU_Configure(&DMAC_CH1_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DMAC_CH1_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DMAC_CH2_CH
    ret = PPU_Configure(&DMAC_CH2_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DMAC_CH2_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_DMAC_CH3_CH
    ret = PPU_Configure(&DMAC_CH3_CH_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&DMAC_CH3_CH_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_EFUSE_CTL
    ret = PPU_Configure(&EFUSE_CTL_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&EFUSE_CTL_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_EFUSE_DATA
    ret = PPU_Configure(&EFUSE_DATA_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&EFUSE_DATA_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PROFILE
    ret = PPU_Configure(&PROFILE_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PROFILE_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT0_PRT
    ret = PPU_Configure(&HSIOM_PRT0_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT0_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT1_PRT
    ret = PPU_Configure(&HSIOM_PRT1_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT1_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT2_PRT
    ret = PPU_Configure(&HSIOM_PRT2_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT2_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT3_PRT
    ret = PPU_Configure(&HSIOM_PRT3_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT3_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT4_PRT
    ret = PPU_Configure(&HSIOM_PRT4_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT4_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT5_PRT
    ret = PPU_Configure(&HSIOM_PRT5_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT5_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT6_PRT
    ret = PPU_Configure(&HSIOM_PRT6_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT6_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT7_PRT
    ret = PPU_Configure(&HSIOM_PRT7_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT7_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT8_PRT
    ret = PPU_Configure(&HSIOM_PRT8_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT8_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT9_PRT
    ret = PPU_Configure(&HSIOM_PRT9_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT9_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT10_PRT
    ret = PPU_Configure(&HSIOM_PRT10_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT10_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT11_PRT
    ret = PPU_Configure(&HSIOM_PRT11_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT11_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT12_PRT
    ret = PPU_Configure(&HSIOM_PRT12_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT12_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT13_PRT
    ret = PPU_Configure(&HSIOM_PRT13_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT13_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_PRT14_PRT
    ret = PPU_Configure(&HSIOM_PRT14_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_PRT14_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_AMUX
    ret = PPU_Configure(&HSIOM_AMUX_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_AMUX_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_HSIOM_MON
    ret = PPU_Configure(&HSIOM_MON_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&HSIOM_MON_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT0_PRT
    ret = PPU_Configure(&GPIO_PRT0_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT0_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT1_PRT
    ret = PPU_Configure(&GPIO_PRT1_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT1_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT2_PRT
    ret = PPU_Configure(&GPIO_PRT2_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT2_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT3_PRT
    ret = PPU_Configure(&GPIO_PRT3_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT3_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT4_PRT
    ret = PPU_Configure(&GPIO_PRT4_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT4_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT5_PRT
    ret = PPU_Configure(&GPIO_PRT5_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT5_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT6_PRT
    ret = PPU_Configure(&GPIO_PRT6_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT6_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT7_PRT
    ret = PPU_Configure(&GPIO_PRT7_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT7_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT8_PRT
    ret = PPU_Configure(&GPIO_PRT8_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT8_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT9_PRT
    ret = PPU_Configure(&GPIO_PRT9_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT9_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT10_PRT
    ret = PPU_Configure(&GPIO_PRT10_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT10_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT11_PRT
    ret = PPU_Configure(&GPIO_PRT11_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT11_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT12_PRT
    ret = PPU_Configure(&GPIO_PRT12_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT12_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT13_PRT
    ret = PPU_Configure(&GPIO_PRT13_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT13_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT14_PRT
    ret = PPU_Configure(&GPIO_PRT14_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT14_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT0_CFG
    ret = PPU_Configure(&GPIO_PRT0_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT0_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT1_CFG
    ret = PPU_Configure(&GPIO_PRT1_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT1_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT2_CFG
    ret = PPU_Configure(&GPIO_PRT2_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT2_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT3_CFG
    ret = PPU_Configure(&GPIO_PRT3_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT3_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT4_CFG
    ret = PPU_Configure(&GPIO_PRT4_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT4_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT5_CFG
    ret = PPU_Configure(&GPIO_PRT5_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT5_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT6_CFG
    ret = PPU_Configure(&GPIO_PRT6_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT6_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT7_CFG
    ret = PPU_Configure(&GPIO_PRT7_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT7_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT8_CFG
    ret = PPU_Configure(&GPIO_PRT8_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT8_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT9_CFG
    ret = PPU_Configure(&GPIO_PRT9_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT9_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT10_CFG
    ret = PPU_Configure(&GPIO_PRT10_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT10_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT11_CFG
    ret = PPU_Configure(&GPIO_PRT11_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT11_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT12_CFG
    ret = PPU_Configure(&GPIO_PRT12_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT12_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT13_CFG
    ret = PPU_Configure(&GPIO_PRT13_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT13_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_PRT14_CFG
    ret = PPU_Configure(&GPIO_PRT14_CFG_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_PRT14_CFG_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_GPIO
    ret = PPU_Configure(&GPIO_GPIO_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_GPIO_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_GPIO_TEST
    ret = PPU_Configure(&GPIO_TEST_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&GPIO_TEST_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SMARTIO_PRT8_PRT
    ret = PPU_Configure(&SMARTIO_PRT8_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SMARTIO_PRT8_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SMARTIO_PRT9_PRT
    ret = PPU_Configure(&SMARTIO_PRT9_PRT_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SMARTIO_PRT9_PRT_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_LPCOMP
    ret = PPU_Configure(&LPCOMP_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&LPCOMP_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_CSD0
    ret = PPU_Configure(&CSD0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&CSD0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_TCPWM0
    ret = PPU_Configure(&TCPWM0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&TCPWM0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_TCPWM1
    ret = PPU_Configure(&TCPWM1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&TCPWM1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_LCD0
    ret = PPU_Configure(&LCD0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&LCD0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_USBFS0
    ret = PPU_Configure(&USBFS0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&USBFS0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SMIF0
    ret = PPU_Configure(&SMIF0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SMIF0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SDHC0
    ret = PPU_Configure(&SDHC0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SDHC0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SDHC1
    ret = PPU_Configure(&SDHC1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SDHC1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB0
    ret = PPU_Configure(&SCB0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB1
    ret = PPU_Configure(&SCB1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB2
    ret = PPU_Configure(&SCB2_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB2_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB3
    ret = PPU_Configure(&SCB3_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB3_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB4
    ret = PPU_Configure(&SCB4_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB4_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB5
    ret = PPU_Configure(&SCB5_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB5_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB6
    ret = PPU_Configure(&SCB6_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB6_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB7
    ret = PPU_Configure(&SCB7_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB7_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB8
    ret = PPU_Configure(&SCB8_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB8_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB9
    ret = PPU_Configure(&SCB9_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB9_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB10
    ret = PPU_Configure(&SCB10_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB10_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB11
    ret = PPU_Configure(&SCB11_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB11_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_SCB12
    ret = PPU_Configure(&SCB12_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&SCB12_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_PDM0
    ret = PPU_Configure(&PDM0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&PDM0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_I2S0
    ret = PPU_Configure(&I2S0_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&I2S0_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

#if RTE_MS_PPU_I2S1
    ret = PPU_Configure(&I2S1_PPU_Resources);
    if (ret) {
        printf("PPU_Configure(&I2S1_PPU_Resources) returned %#x\n", ret);
    }
    assert(ret == CY_PROT_SUCCESS);
#endif

    __DSB();
    __ISB();
}
