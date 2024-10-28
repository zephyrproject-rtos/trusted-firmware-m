/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

/**
 * \file host_base_address.h
 * \brief This file defines the host memory map addresses accessed by RSE.
 */

#ifndef __HOST_BASE_ADDRESS_H__
#define __HOST_BASE_ADDRESS_H__

#include "platform_base_address.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

#define RSE_ATU_PAGE_SIZE (0x2000U) /* 8KB */

/* Host addresses */
#define AP_BOOT_SRAM_BASE   0x0UL        /* AP initial boot SRAM base address */
#define AP_BOOT_SRAM_SIZE   0x80000U     /* 512KB */

#define HOST_UART_BASE     0x7FF80000UL /* Host UART base address */
#define HOST_UART_SIZE     0x2000U      /* 8KB */

#define HOST_FLASH0_BASE   0x08000000UL /* Host flash base address */
#define HOST_FLASH0_SIZE   0x4000000    /* 64 MiB */

#define AP_TRUSTED_SRAM_BASE  0x4000000UL /* AP trusted SRAM base address */
#define AP_SHARED_RAM_BASE    AP_TRUSTED_SRAM_BASE

#define PLAT_SCP_AP_SDS_SIZE          0xDC0U  /* 3520 bytes */
#define PLAT_SCP_SCMI_S_PAYLOAD_SIZE  0x100U  /*  256 bytes */
#define PLAT_RSE_AP_SDS_BASE  (AP_SHARED_RAM_BASE \
                               + PLAT_SCP_AP_SDS_SIZE \
                               + PLAT_SCP_SCMI_S_PAYLOAD_SIZE)
#define PLAT_RSE_AP_SDS_ATU_MAPPING_BASE (ALIGN_DOWN(PLAT_RSE_AP_SDS_BASE, \
                                                RSE_ATU_PAGE_SIZE))
#define PLAT_RSE_AP_SDS_SIZE  (64U)
#define PLAT_RSE_AP_SDS_ATU_MAPPING_SIZE ((ALIGN_UP(PLAT_RSE_AP_SDS_BASE +    \
                                                        PLAT_RSE_AP_SDS_SIZE, \
                                                    RSE_ATU_PAGE_SIZE)) -     \
                                          PLAT_RSE_AP_SDS_ATU_MAPPING_BASE)

/* Temporary ATU mapping location. Placed directly after the last address
 * currently used for logical mapping in the RSE */
#define TEMPORARY_ATU_MAPPING_BASE (HOST_ACCESS_BASE_S + 5 * HOST_IMAGE_MAX_SIZE)

/* There is currently no single location where the ATU regions
 * are defined, so choose an arbitrary region which isn't
 * currently being used elsewhere in RSE */
#define TEMPORARY_ATU_MAPPING_REGION_ID (10)

#define RSE_ATU_AP_BASE             (0x00000000000000UL)
#define HOST_STAGING_MEM_BASE       (RSE_ATU_AP_BASE + 0x80000000UL)

#define SCP_BOOT_SRAM_BASE  0x1000000000000 /* SCP initial boot SRAM base address */
#define SCP_BOOT_SRAM_SIZE  0x20000U        /* 128KB */

#endif  /* __HOST_BASE_ADDRESS_H__ */
