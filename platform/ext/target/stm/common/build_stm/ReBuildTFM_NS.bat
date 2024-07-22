set WORK_DIR=%CD%
set BUILD_DIR=%WORK_DIR%\iar
set BUILD_TFM=build_ns

cd %BUILD_DIR%
del /s/q %BUILD_TFM%

set WORK_DIR=%WORK_DIR:\=/%
set BUILD_S=%WORK_DIR%/iar/build_s

set TFM_SOURCE=%WORK_DIR%/trusted-firmware-m
set QCBOR=-DQCBOR_PATH=%WORK_DIR%/QCBOR
set TFMTEST_SRC=%WORK_DIR%/tf-m-tests/tests_reg

set TOOL_CHAIN=-DTFM_TOOLCHAIN_FILE=%BUILD_S%/api_ns/cmake/toolchain_ns_IARARM.cmake

cmake -S %TFM_SOURCE% %TFMTEST_SRC% -B %BUILD_TFM% -GNinja -DCONFIG_SPE_PATH=%BUILD_S%/api_ns %TOOL_CHAIN%
ninja -C %BUILD_TFM% -j12
pause