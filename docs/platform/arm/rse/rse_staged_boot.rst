###############
RSE Staged Boot
###############

The default boot flow for RSE platform is mentioned in the doc  :doc:`RSE introduction </platform/arm/rse/readme>`

RSE can be easily adapted to various platform topologies. For example, in a platform with multiple RSE(s),
(where each RSE resides on separate die loading different subsystems), not all the RSE(s) would have access to
external flash for loading the images. In such scenarios, a staged boot is required.
During the staged boot process, RSE with access to external flash would copy the entire GPT to the
staged (local/common) RAM area. Then each RSE would parse the GPT/FIP from staged area and
continue with normal boot flow.

To enable the staged boot by BL2, parameter ``RSE_BL2_ENABLE_IMAGE_STAGING`` must be enabled.
The staging area addresses are defined the staging_config.h header file.

--------------

*Copyright (c) 2024, Arm Limited. All rights reserved.*