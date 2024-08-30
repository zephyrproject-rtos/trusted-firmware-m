/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __GIC_720AE_DRV_H__
#define __GIC_720AE_DRV_H__

#include "gic_720ae_reg.h"
#include <stdint.h>

/* Structure for GIC multiple view device */
struct gic_mv_dev_t {
    uint16_t  view_num;
    uint16_t  rdist_num;
    uint32_t  rdist_sz;
    uintptr_t gicd_base;
    uintptr_t gicr_base;
};

/* Structure for GIC Multiple View PE to View mapping */
struct gic_mv_p2v_map {
    uint32_t mpid;
    uint8_t  view;
};

/* Structure for GIC Multiple View SPI to View mapping */
struct gic_mv_i2v_map {
    uint16_t spi;
    uint8_t  view;
};

/*
 * Probe the GIC device and detect the Multiple View feature.
 */
int gic_multiple_view_device_probe(struct gic_mv_dev_t *dev,
                                   uintptr_t base_addr,
                                   uint16_t view_num,
                                   uint16_t rdist_num);

/*
 * Configure the PEs and Views mapping through a P2V map.
 */
int gic_multiple_view_config_pe(struct gic_mv_dev_t *dev,
                                struct gic_mv_p2v_map *pe_map,
                                uint32_t num);
/*
 * Configure the SPIs and Views mapping through an I2V map.
 */
int gic_multiple_view_config_spi(struct gic_mv_dev_t *dev,
                                 struct gic_mv_i2v_map *spi_map,
                                 uint32_t num);

#endif /* __GIC_720AE_DRV_H__ */
