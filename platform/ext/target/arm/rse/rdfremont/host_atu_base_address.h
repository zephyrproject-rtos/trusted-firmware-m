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

#include "platform_base_address.h"
#include "size_defs.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

#define RSE_ATU_BASE_ID 0

enum rse_atu_ids {
    /* ATU region ID for programming NI-Tower */
    RSE_ATU_NI_TOWER_ID = RSE_ATU_BASE_ID,
};

/*
 * ATU controller enforces a minimum size and all regions are restricted to
 * align with it
 */
#define RSE_ATU_PAGE_SIZE       0x2000U /* 8KB */

/* ATU region mapping to access System Control NI-Tower */
#define HOST_NI_TOWER_BASE      HOST_ACCESS_BASE_S
#define HOST_NI_TOWER_SIZE      ALIGN_UP(0x1000000U, RSE_ATU_PAGE_SIZE)
#define HOST_NI_TOWER_ATU_ID    RSE_ATU_NI_TOWER_ID

#endif  /* __HOST_ATU_BASE_ADDRESS_H__ */
