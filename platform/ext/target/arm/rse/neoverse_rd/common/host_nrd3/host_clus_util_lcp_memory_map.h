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
 * \file host_clus_util_lcp_memory_map.h
 * \brief This file contains addresses that are defined in the compute
 *        subsystem (CSS) LCP and cluster utility memory map.
 */

#ifndef __HOST_CLUS_UTIL_LCP_MEMORY_MAP_H__
#define __HOST_CLUS_UTIL_LCP_MEMORY_MAP_H__

/* LCP memory map base address */
#define HOST_LCP_MMAP_PHYS_BASE                     0x00000000ULL
/* LCP memory map end address */
#define HOST_LCP_MMAP_PHYS_LIMIT                    0xFFFFFFFFULL

/*
 * Following are address offsets for each cluster's memory map.
 * phys_address = 0x200000000 + (CLUSTER_IDX * 0x200000) + following offsets
 */
/* LCP + SMCF components base address */
#define HOST_CLUS_UTIL_LCP_SMCF_OFF_ADDR_PHYS_BASE    0x00000000ULL
/* LCP + SMCF components end address */
#define HOST_CLUS_UTIL_LCP_SMCF_OFF_ADDR_PHYS_LIMIT   0x000FFFFFULL

/* Cluster control base address */
#define HOST_CLUS_UTIL_CLUS_CTRL_OFF_ADDR_PHYS_BASE   0x00100000ULL
/* Cluster control end address */
#define HOST_CLUS_UTIL_CLUS_CTRL_OFF_ADDR_PHYS_LIMIT  0x0015FFFFULL

/* PPU base address */
#define HOST_CLUS_UTIL_PPU_OFF_ADDR_PHYS_BASE         0x00180000ULL
/* PPU end address */
#define HOST_CLUS_UTIL_PPU_OFF_ADDR_PHYS_LIMIT        0x0018FFFFULL

/* AMU base address */
#define HOST_CLUS_UTIL_AMU_OFF_ADDR_PHYS_BASE         0x00190000ULL
/* AMU end address */
#define HOST_CLUS_UTIL_AMU_OFF_ADDR_PHYS_LIMIT        0x0019FFFFULL

/* RAS base address */
#define HOST_CLUS_UTIL_RAS_OFF_ADDR_PHYS_BASE         0x001A0000ULL
/* RAS end address */
#define HOST_CLUS_UTIL_RAS_OFF_ADDR_PHYS_LIMIT        0x001AFFFFULL

/* MPMM base address */
#define HOST_CLUS_UTIL_MPMM_OFF_ADDR_PHYS_BASE        0x001B0000ULL
/* MPMM end address */
#define HOST_CLUS_UTIL_MPMM_OFF_ADDR_PHYS_LIMIT       0x001BFFFFULL

#endif /* __HOST_CLUS_UTIL_LCP_MEMORY_MAP_H__ */
