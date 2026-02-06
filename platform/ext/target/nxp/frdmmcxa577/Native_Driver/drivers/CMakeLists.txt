# Copyright 2025 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

mcux_add_cmakelists(${SdkRootDirPath}/${device_root}/MCX/MCXA/MCXA577/drivers/romapi)

if (CONFIG_MCUX_COMPONENT_driver.clock)
    mcux_component_version(2.0.0)
    mcux_add_source( SOURCES fsl_clock.c fsl_clock.h )
    mcux_add_include( INCLUDES . )
endif()

if (CONFIG_MCUX_COMPONENT_driver.edma_soc)
    mcux_component_version(2.0.0)
    mcux_add_source( SOURCES fsl_edma_soc.c fsl_edma_soc.h )
    mcux_add_include( INCLUDES . )
endif()

if (CONFIG_MCUX_COMPONENT_driver.inputmux_connections)
    mcux_component_version(2.0.0)
    mcux_add_source( SOURCES fsl_inputmux_connections.h )
    mcux_add_include( INCLUDES . )
endif()

if (CONFIG_MCUX_COMPONENT_driver.reset)
    mcux_component_version(2.4.0)
    mcux_add_source( SOURCES fsl_reset.c fsl_reset.h )
    mcux_add_include( INCLUDES . )
endif()

if (CONFIG_MCUX_COMPONENT_driver.trdc_soc)
    mcux_component_version(2.0.0)
    mcux_add_source( SOURCES fsl_trdc_soc.h )
    mcux_add_include( INCLUDES . )
endif()
