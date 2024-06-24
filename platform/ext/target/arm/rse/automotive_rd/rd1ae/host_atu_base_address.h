/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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

/**
 * \file host_atu_base_address.h
 * \brief This file defines the host memory ATU addresses.
 */

#ifndef __HOST_ATU_BASE_ADDRESS_H__
#define __HOST_ATU_BASE_ADDRESS_H__

#include "image_size_defs.h"
#include "platform_base_address.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

#define RSE_ATU_BASE_ID 0

enum rse_atu_ids {
    /*
     * ATU regions for loading firmware in BL2. Reused per firmware. Not used
     * outside BL2.
     */

    /* ID to use for region loading the header of an image */
    RSE_ATU_IMG_HDR_LOAD_ID = RSE_ATU_BASE_ID,
    /* ID to use for region loading the rest of an image */
    RSE_ATU_IMG_CODE_LOAD_ID,
    /* ID to use for region initializing firmware */
    RSE_ATU_FW_INIT_ID,

    /* ATU region ID for programming NI-Tower */
    RSE_ATU_NI_TOWER_ID,
    /* ATU region ID for programming GIC-720AE */
    RSE_ATU_GIC_ID,
    /* ATU region ID for AP secure flash */
    RSE_ATU_AP_FLASH_ID,
};

/*
 * ATU controller enforces a minimum size and all regions are restricted to
 * align with it
 */
#define RSE_ATU_PAGE_SIZE       0x2000U /* 8KB */

/*
 * In flash the image is layered out like so:
 *
 * ┌────────┬──────┬─────────┐
 * │ HEADER │ CODE │ TRAILER │
 * ├────────┴──────┴─────────┤
 * │ Flash Image             │
 * └─────────────────────────┘
 *
 * However, when the image is loaded the CODE region needs to be at the start
 * of the memory for the host firmware to load.
 *
 * ┌──────┐
 * │ CODE │
 * ├──────┴─────────┐
 * │ ITCM           │
 * └────────────────┘
 *
 * MCUBoot requires a contiguous copy. To allow this while moving the header to
 * a different memory region, the ATU can be used to remap contiguous logical
 * addresses to different locations.
 *
 * The ATU has a fixed size page which limits where boundaries can be. In order
 * to have the ATU boundary match the header code boundary of the image, the
 * image is aligned in logical memory so the boundaries match.
 *
 *                  ┌───────────┐         ┌─────────────────────────┐
 * Physical Memory  │ Somewhere │         │ ITCM                    │
 *                  └──┬────────┤         ├──────┬─────────┬────────┘
 *                     │ HEADER │         │ CODE │ TRAILER │
 *                     └───▲────┘         └──────┴─▲───────┘
 *                         │                       │
 *                         │                       │
 *                  ┌──────┴─────┬─────────────────┴───────┐
 *  Logical Memory  │ ATU HEADER │ ATU CODE                │
 *                  └───┬────────┼──────┬─────────┬────────┘
 *                      │ HEADER │ CODE │ TRAILER │
 *                      └────────┴──────┴─────────┘
 *
 * If there is space at the end of the ITCM then the header can be placed there
 * effectively wrapping the start of the copy to end.
 *
 *                     ┌──────┬─────────┬────────┐
 *  Bootable Image     │ CODE │ TRAILER │ HEADER │
 *                     ├──────┴─────────┴────────┤
 *                     │ ATU CODE + TRAILER      │
 *  ATU window in host └──────────▲─┬────────────┤
 *  memory space                  │ │ ATU HEADER │
 *                                │ └──────▲─────┘
 *                            ┌───┼────────┘
 *                            │   └────────────┐
 *  ATU window in      ┌──────┴─────┬──────────┴──────────────┐
 *  local memory space │ ATU HEADER │ ATU CODE + TRAILER      │
 *                     ├───┬────────┼──────┬─────────┬────────┘
 *  MCUBoot Image      │   │ HEADER │ CODE │ TRAILER │
 *                     │   ├────────┼──────┴─────────┘
 *                     │   │        │
 *                     │   │        └─────► HOST_<IMG>_IMG_CODE_BASE_S
 *                     │   │
 *                     │   └──────────────► HOST_<IMG>_IMG_HDR_BASE_S
 *                     │
 *                     └──────────────────► HOST_<IMG>_HDR_ATU_WINDOW_BASE_S
 *
 * Alternatively the header and/or trailer could be mapped to a different
 * memory such as the DTCM allowing the total image to be larger than the
 * original memory as long as the code fits. Remapping the trailer as well will
 * require an additional ATU region.
 *
 * Note: Regions not to scale
 */

/*
 * The ATU has a minimum page size of 8K and the page must be aligned to the
 * same sized block. The BL2_HEADER_SIZE is typically smaller than that at 1K.
 * The end of the header needs to align to the end of the ATU so the code can
 * be at the start of the next ATU region. So create an ATU window of 8K and
 * place the header at 7K offset.
 */
#define RSE_IMG_HDR_ATU_WINDOW_SIZE ALIGN_UP(BL2_HEADER_SIZE, RSE_ATU_PAGE_SIZE)

/*
 * RSE ATU Regions for image loading
 * Note: these need to consistent with values used when signing the images.
 * Note: MCUBoot requires that the logical addresses do not overlap.
 */

/* SCP */

/* SCP ATU HEADER logical address start */
#define HOST_SCP_HDR_ATU_WINDOW_BASE_S  HOST_ACCESS_BASE_S
/* SCP Image address start, offset so end of HEADER is at end of ATU HEADER */
#define HOST_SCP_IMG_HDR_BASE_S         (HOST_SCP_HDR_ATU_WINDOW_BASE_S +   \
                                         RSE_IMG_HDR_ATU_WINDOW_SIZE -      \
                                         BL2_HEADER_SIZE)
/* SCP Code region and SCP ATU CODE logical address start */
#define HOST_SCP_IMG_CODE_BASE_S        (HOST_SCP_HDR_ATU_WINDOW_BASE_S +   \
                                         RSE_IMG_HDR_ATU_WINDOW_SIZE)
/* SCP ATU CODE size (aligned size of SCP image) */
#define HOST_SCP_ATU_SIZE               ALIGN_UP(SIZE_DEF_SCP_IMAGE,        \
                                                 RSE_ATU_PAGE_SIZE)
/* SCP HEADER physical address start (mapped to end of SCP ITCM) */
#define HOST_SCP_HDR_PHYS_BASE          (HOST_SCP_PHYS_BASE +               \
                                         HOST_SCP_ATU_SIZE -                \
                                         RSE_IMG_HDR_ATU_WINDOW_SIZE)

/* AP BL2 */

/* AP BL2 ATU HEADER logical address start */
#define HOST_AP_BL2_HDR_ATU_WINDOW_BASE_S (HOST_SCP_IMG_CODE_BASE_S +          \
                                           HOST_SCP_ATU_SIZE)
/* AP BL2 Image address start, offset so end of HEADER at end of ATU HEADER */
#define HOST_AP_BL2_IMG_HDR_BASE_S        (HOST_AP_BL2_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE -       \
                                           BL2_HEADER_SIZE)
/* AP BL2 Code region and SCP ATU CODE logical address start */
#define HOST_AP_BL2_IMG_CODE_BASE_S       (HOST_AP_BL2_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)
/* AP BL2 Shared SRAM physical address start */
#define HOST_AP_BL2_PHYS_BASE             (HOST_AP_SHARED_SRAM_PHYS_BASE +     \
                                           0x42000UL)
/* AP BL2 ATU CODE size (aligned size of SCP image) */
#define HOST_AP_BL2_ATU_SIZE              ALIGN_UP(SIZE_DEF_AP_BL2_IMAGE,      \
                                                   RSE_ATU_PAGE_SIZE)
/* AP BL2 HEADER physical address start (mapped to end of AP ITCM) */
#define HOST_AP_BL2_HDR_PHYS_BASE         (HOST_AP_BL2_PHYS_BASE +             \
                                           HOST_AP_BL2_ATU_SIZE -              \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)

/* SI CL0 */

/* SI CL0 ATU HEADER logical address start */
#define HOST_SI_CL0_HDR_ATU_WINDOW_BASE_S (HOST_AP_BL2_IMG_CODE_BASE_S +       \
                                           HOST_AP_BL2_ATU_SIZE)
/* SI CL0 Image address start, offset so end of HEADER at end of ATU HEADER */
#define HOST_SI_CL0_IMG_HDR_BASE_S        (HOST_SI_CL0_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE -       \
                                           BL2_HEADER_SIZE)
/* SI CL0 Code region and SCP ATU CODE logical address start */
#define HOST_SI_CL0_IMG_CODE_BASE_S       (HOST_SI_CL0_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)
/* SI CL0 Shared SRAM physical address start */
#define HOST_SI_CL0_PHYS_BASE             HOST_SI_CL0_SRAM_PHYS_BASE
/* SI CL0 ATU CODE size (aligned size of SCP image) */
#define HOST_SI_CL0_ATU_SIZE              ALIGN_UP(SIZE_DEF_SI_CL0_IMAGE,      \
                                                   RSE_ATU_PAGE_SIZE)
/* SI CL0 HEADER physical address start (mapped to end of SI CL0 ITCM) */
#define HOST_SI_CL0_HDR_PHYS_BASE         (HOST_SI_CL0_PHYS_BASE +             \
                                           HOST_SI_CL0_ATU_SIZE -              \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)

/* SI CL1 */

/* SI CL1 ATU HEADER logical address start */
#define HOST_SI_CL1_HDR_ATU_WINDOW_BASE_S (HOST_SI_CL0_IMG_CODE_BASE_S +       \
                                           HOST_SI_CL0_ATU_SIZE)
/* SI CL1 Image address start, offset so end of HEADER at end of ATU HEADER */
#define HOST_SI_CL1_IMG_HDR_BASE_S        (HOST_SI_CL1_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE -       \
                                           BL2_HEADER_SIZE)
/* SI CL1 Code region and SCP ATU CODE logical address start */
#define HOST_SI_CL1_IMG_CODE_BASE_S       (HOST_SI_CL1_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)
/* SI CL1 Shared SRAM physical address start */
#define HOST_SI_CL1_PHYS_BASE             HOST_SI_CL1_SRAM_PHYS_BASE
/* SI CL1 ATU CODE size (aligned size of SCP image) */
#define HOST_SI_CL1_ATU_SIZE              ALIGN_UP(SIZE_DEF_SI_CL1_IMAGE,      \
                                                   RSE_ATU_PAGE_SIZE)
/* SI CL1 HEADER physical address start (mapped to end of SI CL1 ITCM) */
#define HOST_SI_CL1_HDR_PHYS_BASE         (HOST_SI_CL1_PHYS_BASE +             \
                                           HOST_SI_CL1_ATU_SIZE -              \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)

/* SI CL2 */

/* SI CL2 ATU HEADER logical address start */
#define HOST_SI_CL2_HDR_ATU_WINDOW_BASE_S (HOST_SI_CL1_IMG_CODE_BASE_S +       \
                                           HOST_SI_CL1_ATU_SIZE)
/* SI CL2 Image address start, offset so end of HEADER at end of ATU HEADER */
#define HOST_SI_CL2_IMG_HDR_BASE_S        (HOST_SI_CL2_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE -       \
                                           BL2_HEADER_SIZE)
/* SI CL2 Code region and SCP ATU CODE logical address start */
#define HOST_SI_CL2_IMG_CODE_BASE_S       (HOST_SI_CL2_HDR_ATU_WINDOW_BASE_S + \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)
/* SI CL2 Shared SRAM physical address start */
#define HOST_SI_CL2_PHYS_BASE             HOST_SI_CL2_SRAM_PHYS_BASE
/* SI CL2 ATU CODE size (aligned size of SCP image) */
#define HOST_SI_CL2_ATU_SIZE              ALIGN_UP(SIZE_DEF_SI_CL2_IMAGE,      \
                                                   RSE_ATU_PAGE_SIZE)
/* SI_CL2 HEADER physical address start (mapped to end of SI CL2 ITCM) */
#define HOST_SI_CL2_HDR_PHYS_BASE         (HOST_SI_CL2_PHYS_BASE +             \
                                           HOST_SI_CL2_ATU_SIZE -              \
                                           RSE_IMG_HDR_ATU_WINDOW_SIZE)

/* Last RSE logical address used for loading images */
#define RSE_IMAGE_LOADING_END             (HOST_SI_CL2_IMG_CODE_BASE_S +       \
                                           HOST_SI_CL2_ATU_SIZE)

/* SCP sysctrl region logical address start */
#define HOST_SCP_INIT_CTRL_BASE_S    RSE_IMAGE_LOADING_END
/* SCP sysctrl region physical address start */
#define HOST_SCP_INIT_CTRL_PHYS_BASE 0x1000050050000ULL
/* SCP sysctrl region ATU size */
#define HOST_SCP_INIT_CTRL_SIZE      ALIGN_UP(0x1000U, RSE_ATU_PAGE_SIZE)
/* SCP sysctrl region ATU id */
#define HOST_SCP_INIT_CTRL_ATU_ID    RSE_ATU_FW_INIT_ID

/*
 * ATU region mapping to access System Control NI-Tower and Peripheral
 * NI-Tower
 */
#define HOST_NI_TOWER_BASE      (HOST_SCP_INIT_CTRL_BASE_S +                \
                                 HOST_SCP_INIT_CTRL_SIZE)
#define HOST_NI_TOWER_SIZE      ALIGN_UP(0x1000000U, RSE_ATU_PAGE_SIZE)
#define HOST_NI_TOWER_ATU_ID    RSE_ATU_NI_TOWER_ID

/* ATU region mapping to access AP secure flash */
#define HOST_AP_FLASH_BASE      (HOST_NI_TOWER_BASE + HOST_NI_TOWER_SIZE)
#define HOST_AP_FLASH_SIZE      ALIGN_UP(AP_BOOT_FLASH_SIZE, RSE_ATU_PAGE_SIZE)
#define HOST_AP_FLASH_PHY_BASE  HOST_AP_MEM_EXP_1_PHYS_BASE
#define HOST_AP_FLASH_ATU_ID    RSE_ATU_AP_FLASH_ID

/* Safety Island GIC view 0 region logical address start */
#define HOST_SI_GIC_VIEW_0_BASE_S       (HOST_ACCESS_BASE_S + 0x8000000U)
/* Safety Island GIC View 0 region physical address start */
#define HOST_SI_GIC_VIEW_0_PHYS_BASE    0x8000030000000ULL
/* Safety Island GIC region ATU size */
#define HOST_SI_GIC_VIEW_SIZE           0x200000U

#endif  /* __HOST_ATU_BASE_ADDRESS_H__ */
