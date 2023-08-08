#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CONFIG_TFM_ENALBE_PROFILING OFF CACHE BOOL "Enable profiling for TF-M")

if(CONFIG_TFM_ENALBE_PROFILING)
    # Profiler source can be found from downloaded or local tf-m-tools repo.
    if(NOT TFM_PROFILING_PATH)
        if (NOT TFM_TOOLS_PATH)
            add_subdirectory(${CMAKE_SOURCE_DIR}/lib/ext/tf-m-tools)
        endif()

        set(TFM_PROFILING_PATH "${TFM_TOOLS_PATH}/profiling" CACHE PATH "Profiler tool source path")
    endif()

    include(${TFM_PROFILING_PATH}/profiling_cases/config.cmake)
endif()
