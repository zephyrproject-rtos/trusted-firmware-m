--------
STM32WBA
--------

TF-M is supported on STM32WBA series

https://www.st.com/en/microcontrollers-microprocessors/stm32wba-series.html

Directory content
^^^^^^^^^^^^^^^^^

- stm/common/stm32wbaxx/stm32wbaxx_hal:
   Content from https://github.com/STMicroelectronics/stm32wbaxx_hal_driver (HAL version - Tags V1.6.0 )

- stm/common/stm32wbaxx/Device:
   Content from https://github.com/STMicroelectronics/cmsis_device_wba

- stm/common/stm32wbaxx/secure:
   stm32wbaxx Secure porting adaptation from https://github.com/STMicroelectronics/STM32CubeWBA.git

- stm/common/stm32wbaxx/boards:
   Adaptation and tools specific to stm32 board using stm32wbaxx device from https://github.com/STMicroelectronics/STM32CubeWBA.git

- stm/common/stm32wbaxx/CMSIS_Driver:
   Flash and uart driver for stm32wbaxx platform

- stm/common/stm32wbaxx/Native_Driver:
   Random generator and tickless implementation

Specific Software Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

STM32CubeProgrammer is required.(see https://www.st.com/en/development-tools/stm32cubeprog.html)


Limitations to Consider When Using the Platform
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M Supported without BL1/BL2.

By default, TFM_OTP_DEFAULT_PROVISIONING and DEFAULT_SHARED_DATA switches are enabled in TF-M,
to use dummy data in OTP and SRAM shared_data areas as BL2 not supported.

-------------

*Copyright (c) 2024 STMicroelectronics. All rights reserved.*
*SPDX-License-Identifier: BSD-3-Clause*