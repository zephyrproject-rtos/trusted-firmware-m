#!/bin/bash -
#******************************************************************************
#  * @attention
#  *
#  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
#  * All rights reserved.</center></h2>
#  *
#  * This software component is licensed by ST under BSD 3-Clause license,
#  * the "License"; You may not use this file except in compliance with the
#  * License. You may obtain a copy of the License at:
#  *                        opensource.org/licenses/BSD-3-Clause
#  *
#  ******************************************************************************

echo "regression script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
# remove write protection
secbootadd0=0x180030
connect="-c port=SWD mode=UR --hardRst"
connect_no_reset="-c port=SWD mode=HotPlug"
rdp_0="-ob RDP=0xAA TZEN=1"
remove_bank1_protect="-ob SECWM1_PSTRT=127 SECWM1_PEND=0 WRP1A_PSTRT=127 WRP1A_PEND=0 WRP1B_PSTRT=127 WRP1B_PEND=0"
remove_bank2_protect="-ob SECWM2_PSTRT=127 SECWM2_PEND=0 WRP2A_PSTRT=127 WRP2A_PEND=0 WRP2B_PSTRT=127 WRP2B_PEND=0"
erase_all="-e all"
remove_hdp_protection="-ob HDP1_PEND=0 HDP1EN=0 HDP2_PEND=0 HDP2EN=0"
default_ob="-ob SRAM2_RST=0 SECBOOTADD0="$secbootadd0" DBANK=1 SWAP_BANK=0 SECWM1_PSTRT=0 SECWM1_PEND=127 SECWM2_PSTRT=127 SECWM2_PEND=0"
echo "Regression to RDP 0 and  tzen=1"
$stm32programmercli $connect_no_reset $rdp_0
echo "Remove bank1 protection and erase all"
$stm32programmercli $connect $remove_bank1_protect $erase_all
echo "Remove bank2 protection and erase all"
$stm32programmercli $connect $remove_bank2_protect $erase_all
echo "Remove hdp protection"
$stm32programmercli $connect_no_reset $remove_hdp_protection
echo "Set default OB (dual bank  secure entry point, bank 1 full secure"
$stm32programmercli $connect_no_reset $default_ob
echo "regression script Done"
