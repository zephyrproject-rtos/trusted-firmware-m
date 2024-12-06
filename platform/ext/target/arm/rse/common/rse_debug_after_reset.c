/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "platform_regs.h"
#include "rse_debug_after_reset.h"
#include "runtime_shared_data.h"
#include "tfm_hal_device_header.h"

static void set_debug_state_and_notify_boot(void)
{
    uint32_t reg_value;
    struct rse_sysctrl_t *rse_sysctrl = (struct rse_sysctrl_t *)RSE_SYSCTRL_BASE_S;

    /* The SWSYN shall be written along with SWRESETREQ (bit position 5)
     * otherwise it is ignored
     */
    reg_value = (1 << SWSYN_DEBUG_STATE_IN_BOOT_BIT_POS) | (1 << 5);

    /* Raise reset request to enter debug state */
    __DSB();
    rse_sysctrl->swreset = reg_value;
    __DSB();

    while(1) {
        __NOP();
    }
}

int rse_debug_after_reset(const uint8_t *permissions_mask,
                          size_t mask_len)
{
    int rc;

    if (permissions_mask == NULL) {
        return -1;
    }

    /* Copy permissions to the the shared area before reset */
    rc = runtime_add_data_to_shared_area(TLV_RUNTIME_MAJOR_ADAC,
                                         0,
                                         permissions_mask,
                                         mask_len);
    if (rc != 0) {
        return -1;
    }

    set_debug_state_and_notify_boot();

    return 0;
}