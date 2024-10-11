/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

#ifndef __LCM_OTP_LAYOUT_H__
#define __LCM_OTP_LAYOUT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Life Cycle Manager (LCM) OTP data layout (hardware controlled part)
 *
 */
struct lcm_otp_hardware_area_t {
    volatile uint32_t huk[8];         /*!< Hardware Unique Key*/
    volatile uint32_t guk[8];         /*!< Group Unique Key */
    volatile uint32_t kp_cm[8];       /*!< Provisioning key, chip manufacturing (CM) LCS */
    volatile uint32_t kce_cm[8];      /*!< Code encryption key, chip manufacturing (CM) LCS */
    volatile uint32_t kp_dm[8];       /*!< Provisioning key, device manufacturing (DM) LCS */
    volatile uint32_t kce_dm[8];      /*!< Code encryption key, device manufacturing (DM) LCS */
    volatile uint32_t rotpk[8];       /*!< Root Of Trust Public key */
    volatile uint32_t tp_mode_config; /*!< TP mode config, read to determine the TP mode, writable if CM LCS and TP mode Virgin
                                       *   0x0000_0000: Virgin, 0x0000_FFFF: TCI, 0xFFFF_0000: PCI, otherwise: invalid
                                       */
    volatile uint32_t cm_config_1;    /*!< CM config 1: Each byte (LSB to MSB) contains number of "0" bits in HUK, GUK, KP_CM, KCE_CM */
    volatile uint32_t cm_config_2;    /*!< CM config 2: LSB contains number of "0" bits for ROTPK, bytes 1 and 2 contain the
                                       *   General Purpose Persistent Configuration (GPPC) flags
                                       */
    volatile uint32_t dm_config;      /*!< DM config: from LSB, each byte contains number of "0" bits in KP_DM and KCE_DM */
    volatile uint32_t cm_rma_flag;    /*!< 0x0000_0000 CM RMA state disabled, 0xFFFF_FFFF CM RMA state enabled */
    volatile uint32_t dm_rma_flag;    /*!< 0x0000_0000 DM RMA state disabled, 0xFFFF_FFFF DM RMA state enabled */
};

/**
 * @brief Life Cycle Manager (LCM) OTP data layout, full view
 *
 * @note From offset 0x00F8 up to 0xEFFF it contains User defined data. The actual size allocated
 *       depends on the platform implementation
 *
 * @note It contains the \ref struct lcm_otp_hardware_area_t anonymously
 */
struct lcm_otp_layout_t {
    struct {
        volatile uint32_t huk[8];         /*!< Hardware Unique Key*/
        volatile uint32_t guk[8];         /*!< Group Unique Key */
        volatile uint32_t kp_cm[8];       /*!< Provisioning key, chip manufacturing (CM) LCS */
        volatile uint32_t kce_cm[8];      /*!< Code encryption key, chip manufacturing (CM) LCS */
        volatile uint32_t kp_dm[8];       /*!< Provisioning key, device manufacturing (DM) LCS */
        volatile uint32_t kce_dm[8];      /*!< Code encryption key, device manufacturing (DM) LCS */
        volatile uint32_t rotpk[8];       /*!< Root Of Trust Public key */
        volatile uint32_t tp_mode_config; /*!< TP mode config, read to determine the TP mode, writable if CM LCS and TP mode Virgin
                                           *   0x0000_0000: Virgin, 0x0000_FFFF: TCI, 0xFFFF_0000: PCI, otherwise: invalid
                                           */
        volatile uint32_t cm_config_1;    /*!< CM config 1: Each byte (LSB to MSB) contains number of "0" bits in HUK, GUK, KP_CM, KCE_CM */
        volatile uint32_t cm_config_2;    /*!< CM config 2: LSB contains number of "0" bits for ROTPK, bytes 1 and 2 contain the
                                           *   General Purpose Persistent Configuration (GPPC) flags
                                           */
        volatile uint32_t dm_config;      /*!< DM config: from LSB, each byte contains number of "0" bits in KP_DM and KCE_DM */
        volatile uint32_t cm_rma_flag;    /*!< 0x0000_0000 CM RMA state disabled, 0xFFFF_FFFF CM RMA state enabled */
        volatile uint32_t dm_rma_flag;    /*!< 0x0000_0000 DM RMA state disabled, 0xFFFF_FFFF DM RMA state enabled */
    };

    volatile uint32_t user_data[];        /*!< From 0x00F8 up to 0xEFFF, platform defined */
};

#ifdef __cplusplus
}
#endif

#endif /* __LCM_OTP_LAYOUT_H__ */
