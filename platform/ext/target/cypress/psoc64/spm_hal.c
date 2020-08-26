/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <string.h>

#include "tfm_spm_hal.h"
#include "tfm_hal_platform.h"

#include "device_definition.h"
#include "region_defs.h"
#include "utilities.h"
#include "spe_ipc_config.h"
#include "target_cfg.h"
#include "tfm_multi_core.h"
#include "tfm_platform_core_api.h"
#include "log/tfm_log.h"

#include "cycfg.h"
#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_ipc_drv.h"
#include "cy_prot.h"
#include "cy_pra.h"
#include "pc_config.h"
#include "driver_dap.h"
#include "region.h"

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

enum tfm_plat_err_t tfm_spm_hal_init_isolation_hw(void)
{
    enum tfm_plat_err_t ret;

    Cy_PDL_Init(CY_DEVICE_CFG);

    ret = smpu_init_cfg();

    if (ret == TFM_PLAT_ERR_SUCCESS) {
        ret = ppu_init_cfg();
    }

    if (ret == TFM_PLAT_ERR_SUCCESS) {
        ret = bus_masters_cfg();
    }

    return ret;
}

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
        uint32_t partition_idx,
        const struct tfm_spm_partition_platform_data_t *platform_data)
{
    (void) partition_idx; /* Unused parameter */
    if (!platform_data) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }
    /* TBD */

    return TFM_PLAT_ERR_SUCCESS;
}

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_spm_hal_get_ns_MSP(void)
{
    return *((uint32_t *)memory_regions.non_secure_code_start);
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start+ 4));
}

void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    smpu_print_config();

    if (cy_access_port_control(CY_CM4_AP, CY_AP_EN) == 0) {
        /* The delay is required after Access port was enabled for
        * debugger/programmer to connect and set TEST BIT */
        Cy_SysLib_Delay(100);
        LOG_MSG("Enabled CM4_AP DAP control\r\n");
    }

    LOG_MSG("Starting Cortex-M4 at 0x%x\r\n", start_addr);
    Cy_SysEnableCM4(start_addr);
}

void tfm_spm_hal_wait_for_ns_cpu_ready(void)
{
    uint32_t data;
    cy_en_ipcdrv_status_t status;

    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT),
                                0, IPC_RX_INT_MASK);

    status = Cy_IPC_Drv_SendMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(IPC_TX_CHAN),
                                 IPC_TX_NOTIFY_MASK, IPC_SYNC_MAGIC);
    while (1)
    {
        status = (cy_en_ipcdrv_status_t) Cy_IPC_Drv_GetInterruptStatusMasked(
                        Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT));
        status >>= CY_IPC_NOTIFY_SHIFT;
        if (status & IPC_RX_INT_MASK) {
            Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(
                                      IPC_RX_INTR_STRUCT),
                                      0, IPC_RX_INT_MASK);

            status = Cy_IPC_Drv_ReadMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(
                                            IPC_RX_CHAN),
                                            &data);
            if (status == CY_IPC_DRV_SUCCESS) {
                Cy_IPC_Drv_ReleaseNotify(Cy_IPC_Drv_GetIpcBaseAddress(IPC_RX_CHAN),
                                         IPC_RX_RELEASE_MASK);
                if (data == ~IPC_SYNC_MAGIC) {
                    LOG_MSG("\n\rCores sync success.\r\n");
                    break;
                }
            }
        }
    }
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority)
{
    uint32_t quantized_priority = priority >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(irq_line, quantized_priority);

    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_get_mem_security_attr(const void *p, size_t s,
                                       struct security_attr_info_t *p_attr)
{
    /*
     * FIXME
     * Need to check if the memory region is valid according to platform
     * specific memory mapping.
     */

    /*
     * TODO
     * Currently only check static memory region layout to get security
     * information.
     * Check of hardware SMPU configuration can be added.
     */
    tfm_get_mem_region_security_attr(p, s, p_attr);
}

void tfm_spm_hal_get_secure_access_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr)
{
    uint32_t pc;

    /*
     * FIXME
     * Need to check if the memory region is valid according to platform
     * specific memory mapping.
     */

    /*
     * TODO
     * Currently only check static memory region layout to get attributes.
     * Check of secure memory protection configuration from hardware can be
     * added.
     */
    tfm_get_secure_mem_region_attr(p, s, p_attr);

    pc = Cy_Prot_GetActivePC(CPUSS_MS_ID_CM0);
    /* Check whether the current active PC is configured as the expected one .*/
    if (pc == CY_PROT_SPM_DEFAULT) {
        p_attr->is_mpu_enabled = true;
    } else {
        p_attr->is_mpu_enabled = false;
    }
}

void tfm_spm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /*
     * FIXME
     * Need to check if the memory region is valid according to platform
     * specific memory mapping.
     */

    /*
     * TODO
     * Currently only check static memory region layout to get attributes.
     * Check of non-secure memory protection configuration from hardware can be
     * added.
     */
    tfm_get_ns_mem_region_attr(p, s, p_attr);
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    return nvic_interrupt_enable();
}

void tfm_spm_hal_clear_pending_irq(IRQn_Type irq_line)
{
    NVIC_ClearPendingIRQ(irq_line);
}

void tfm_spm_hal_enable_irq(IRQn_Type irq_line)
{
    NVIC_EnableIRQ(irq_line);
}

void tfm_spm_hal_disable_irq(IRQn_Type irq_line)
{
    NVIC_DisableIRQ(irq_line);
}

enum irq_target_state_t tfm_spm_hal_set_irq_target_state(
                                          IRQn_Type irq_line,
                                          enum irq_target_state_t target_state)
{
    (void)irq_line;
    (void)target_state;

    return TFM_IRQ_TARGET_STATE_SECURE;
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void)
{
    return nvic_interrupt_target_state_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_init_debug(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

/* FIXME:
 * Instead of TFM-customized mcuboot, at this moment psoc64 uses
 * Cypress version of it - CypressBootloader (CYBL). CYBL doesn't
 * populate BOOT_TFM_SHARED_DATA.
 * As a temp workaround, mock mcuboot shared data to pass
 * initialization checks.
*/
void mock_tfm_shared_data(void)
{
    const uint32_t mock_data[] = {
        0x00D92016,    0x00071103,     0x00455053,     0x30000911,
        0x302E302E,    0x00081102,     0x00000000,     0x00241108,
        0x6C170A97,    0x5645665E,     0xDB6E2BA6,     0xA4FF4D74,
        0xFD34D7DB,    0x67449A82,     0x75FD0930,     0xAA15A9F9,
        0x000A1109,    0x32414853,     0x11013635,     0xE6BF0024,
        0x26886FD8,    0xFB97FFF4,     0xFBE6C496,     0x463E99C4,
        0x5D56FC19,    0x34DF6AA2,     0x9A4829C3,     0x114338DC,
        0x534E0008,    0x11404550,     0x2E300009,     0x42302E30,
        0x00000811,    0x48000000,     0x7E002411,     0x5FD9229A,
        0xE9672A5F,    0x31AAE1EA,     0x8514D772,     0x7F3B26BC,
        0x2C7EF27A,    0x9C6047D2,     0x4937BB9F,     0x53000A11,
        0x35324148,    0x24114136,     0xCA60B300,     0x6B8CC9F5,
        0x82482A94,    0x23489DFA,     0xA966B1EF,     0x4A6E6AEF,
        0x19197CA3,    0xC0CC1FED,     0x00000049,     0x00000000
    };
    uint32_t *boot_data = (uint32_t*)BOOT_TFM_SHARED_DATA_BASE;
    memcpy(boot_data, mock_data, sizeof(mock_data));
}

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    platform_init();

#if defined(CY_DEVICE_SECURE)
    /* Initialize Protected Register Access driver. */
    Cy_PRA_Init();
#endif /* defined(CY_DEVICE_SECURE) */

    /* FIXME: Use the actual data from mcuboot */
    mock_tfm_shared_data();

    __enable_irq();
    stdio_init();

    return TFM_HAL_SUCCESS;
}
