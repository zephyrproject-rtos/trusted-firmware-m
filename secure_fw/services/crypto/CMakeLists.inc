#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "crypto" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#       MBEDTLS_INSTALL_DIR - directory where mbed TLS headers and libraries can be found.
#       TFM_ROOT_DIR        - root directory of the TF-M repository.
#Outputs:
#       Will modify include directories to make the source compile.
#       ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(CRYPTO_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED ENABLE_CRYPTO)
  message(FATAL_ERROR "Incomplete build configuration: ENABLE_CRYPTO is undefined. ")
endif()

if (ENABLE_CRYPTO)
  if (NOT DEFINED MBEDTLS_INSTALL_DIR)
    message(FATAL_ERROR "Please set MBEDTLS_INSTALL_DIR before including this file.")
  endif()

  if (NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
  endif()

  set (CRYPTO_C_SRC "${CRYPTO_DIR}/crypto_init.c"
                    "${CRYPTO_DIR}/crypto_alloc.c"
                    "${CRYPTO_DIR}/crypto_cipher.c"
                    "${CRYPTO_DIR}/crypto_hash.c"
                    "${CRYPTO_DIR}/crypto_key.c"
                    "${CRYPTO_DIR}/crypto_wrappers.c"
                    "${CRYPTO_DIR}/crypto_utils.c"
      )

  #Append all our source files to global lists.
  list(APPEND ALL_SRC_C ${CRYPTO_C_SRC})
  unset(CRYPTO_C_SRC)

  #Setting include directories
  embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
  embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
  embedded_include_directories(PATH ${MBEDTLS_INSTALL_DIR}/include ABSOLUTE)

else()
  message(FATAL_ERROR "Build system currently doesn't support selectively disabling of a service.")
endif()
