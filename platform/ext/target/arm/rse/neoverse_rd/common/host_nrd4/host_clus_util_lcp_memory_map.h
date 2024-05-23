/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * \file host_clus_util_lcp_memory_map.h
 * \brief This file contains addresses that are defined in the Infra Power
 *        Management Specification's LCP and cluster utility memory map.
 */

#ifndef __HOST_CLUS_UTIL_LCP_MEMORY_MAP_H__
#define __HOST_CLUS_UTIL_LCP_MEMORY_MAP_H__

#define HOST_AP_CLUST_UTIL_SIZE         0x200000U /* 2 MB */
#define HOST_LCP_CLUST_SIZE             0x200000U /* 2 MB */

#ifdef PLAT_SHARED_LCP
/* Base of LCP cluster region (separate from AP util cluster with shared LCP) */
#define HOST_LCP_PHYS_BASE              (HOST_CLUST_UTIL_PHYS_BASE +        \
                                         0x20000000ULL)

#define HOST_AP_CLUST_UTIL_PHYS_BASE(n) (HOST_CLUST_UTIL_PHYS_BASE +        \
                                         ((n) * HOST_AP_CLUST_UTIL_SIZE))
#define HOST_LCP_N_PHYS_BASE(n)         (HOST_LCP_PHYS_BASE +               \
                                         ((n) * HOST_LCP_CLUST_SIZE))
#else
#define HOST_AP_CLUST_UTIL_PHYS_BASE(n) (HOST_CLUST_UTIL_PHYS_BASE + ((n) * \
                                         (HOST_AP_CLUST_UTIL_SIZE +         \
                                          HOST_LCP_CLUST_SIZE)))
#define HOST_LCP_N_PHYS_BASE(n)         (HOST_AP_CLUST_UTIL_PHYS_BASE(n) +  \
                                         HOST_AP_CLUST_UTIL_SIZE)
#endif /* PLAT_SHARED_LCP */

/* LCP extended control base address */
#define HOST_LCP_EXTENDED_CONTROL_BASE                0x00043000ULL
/* LCP extended control end address */
#define HOST_LCP_EXTENDED_CONTROL_LIMIT               0x00043FFFULL

/* LCP ITCM memory map base address */
#define HOST_LCP_ITCM_BASE                            0x00080000ULL
/* LCP ITCM memory map end address */
#define HOST_LCP_ITCM_LIMIT                           0x000BFFFFULL

/* LCP memory map base address */
#define HOST_LCP_MMAP_PHYS_BASE                       0x00000000ULL
/* LCP memory map end address */
#define HOST_LCP_MMAP_PHYS_LIMIT                      0xFFFFFFFFULL

/*
 * Following are address offsets for each cluster's memory map.
 * phys_address = 0x140000000 + (CLUSTER_IDX * 0x200000) + following offsets
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
