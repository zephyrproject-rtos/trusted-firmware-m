#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
find_package(Python3)

############################### Manifest declaration ###########################

get_filename_component(list_name ${TFM_MANIFEST_LIST} NAME)
configure_file(${TFM_MANIFEST_LIST} ${CMAKE_CURRENT_BINARY_DIR}/${list_name})

set(MANIFEST_LISTS ${CMAKE_CURRENT_BINARY_DIR}/${list_name})

if (TFM_NS_REG_TEST OR TFM_S_REG_TEST)
    list(APPEND TEMP_EXTRA_MANIFEST_LISTS ${TFM_TEST_PATH}/test_services/tfm_test_manifest_list.yaml)
endif()

if (TFM_EXTRA_MANIFEST_LIST_FILES)
    list(APPEND TEMP_EXTRA_MANIFEST_LISTS ${TFM_EXTRA_MANIFEST_LIST_FILES})
endif()

# Build up out-of-tree manifest list array
# Each manifest list occupies two elements in out-of-tree manifest list array:
# - Manifest list path under build directory after configure_file()
# - The original path of manifest list in source directory. It can be used to
#   build up manifest file path if manifest file path is a relative one in
#   manifest list.
set(POSTFIX 1)

foreach(MANIFEST_LIST IN LISTS TEMP_EXTRA_MANIFEST_LISTS)
    if (NOT EXISTS ${MANIFEST_LIST})
        message(FATAL_ERROR "Extra manifest list ${MANIFEST_LIST} doesn't exist")
    endif()

    get_filename_component(MANIFEST_LIST_NAME ${MANIFEST_LIST} NAME_WLE)
    set(TEMP_MANIFEST_LIST
        ${CMAKE_CURRENT_BINARY_DIR}/${MANIFEST_LIST_NAME}_${POSTFIX}.yaml)

    configure_file(${MANIFEST_LIST} ${TEMP_MANIFEST_LIST})
    list(APPEND OUT_OF_TREE_MANIFEST_LIST ${TEMP_MANIFEST_LIST})

    get_filename_component(MANIFEST_LIST_PATH ${MANIFEST_LIST} DIRECTORY)
    # Append the manifest list original path.
    # Manifest file path can be a relative one to manifest list.
    list(APPEND OUT_OF_TREE_MANIFEST_LIST ${MANIFEST_LIST_PATH})

    math(EXPR POSTFIX "${POSTFIX} + 1")
endforeach()

if ("${TEST_PSA_API}" STREQUAL "IPC")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tfm_psa_ff_test_manifest_list.yaml
                   ${CMAKE_CURRENT_BINARY_DIR}/tfm_psa_ff_test_manifest_list.yaml)

    set(MANIFEST_LISTS ${MANIFEST_LISTS} ${CMAKE_CURRENT_BINARY_DIR}/tfm_psa_ff_test_manifest_list.yaml)
endif()

############################### File list declaration ##########################

set(GENERATED_FILE_LISTS ${CMAKE_CURRENT_SOURCE_DIR}/tfm_generated_file_list.yaml)
set(GENERATED_FILE_LISTS ${GENERATED_FILE_LISTS} ${TFM_EXTRA_GENERATED_FILE_LIST_PATH})

############################### Dependency generation ##########################

function(parse_field_from_yaml files field output_variable)
    set(${output_variable} "" PARENT_SCOPE)
    foreach(yaml_file ${files})
        # Load the lines that refer to the key we selected
        file(STRINGS ${yaml_file} temp_variable REGEX " *\"${field}\":")
        # Take only the value of the key
        list(TRANSFORM temp_variable REPLACE " *\"${field}\": *" ";")
        # Remove all commas
        list(TRANSFORM temp_variable REPLACE "," "")
        # Remove all quote marks
        list(TRANSFORM temp_variable REPLACE "\"" "")
        set(${output_variable} ${${output_variable}} ${temp_variable} PARENT_SCOPE)
    endforeach()
endfunction()

parse_field_from_yaml("${GENERATED_FILE_LISTS}" template TEMPLATE_FILES)
# Replace relative paths with absolute paths
list(TRANSFORM TEMPLATE_FILES REPLACE "^([^/\\][^:].*)" "${CMAKE_SOURCE_DIR}/\\1")

parse_field_from_yaml("${GENERATED_FILE_LISTS}" output OUTPUT_FILES)
# Replace relative paths with absolute paths
list(TRANSFORM OUTPUT_FILES REPLACE "^([^/\\][^:].*)" "${CMAKE_BINARY_DIR}/generated/\\1")

parse_field_from_yaml("${MANIFEST_LISTS}" manifest MANIFEST_FILES)
# Replace relative paths with absolute paths
list(TRANSFORM MANIFEST_FILES REPLACE "^([^/\\][^:].*)" "${CMAKE_SOURCE_DIR}/\\1")

############################### Command declaration ############################

# Workaround for heap support
if ("${TEST_PSA_API}" STREQUAL "IPC")
    execute_process(
        WORKING_DIRECTORY ${PSA_ARCH_TESTS_PATH}/api-tests
        COMMAND ${Python3_EXECUTABLE} tools/scripts/manifest_update.py
    )
endif()

add_custom_target(tfm_generated_files
    SOURCES ${OUTPUT_FILES}
)

add_custom_command(OUTPUT ${OUTPUT_FILES}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tfm_parse_manifest_list.py
                                  -m ${MANIFEST_LISTS}
                                  -f ${GENERATED_FILE_LISTS}
                                  -o ${CMAKE_BINARY_DIR}/generated
                                  -e ${OUT_OF_TREE_MANIFEST_LIST}
    DEPENDS ${TEMPLATE_FILES} ${MANIFEST_FILES}
    DEPENDS ${MANIFEST_LISTS}
)

# The files need to be generated before cmake will allow them to be used as
# sources. Due to issue with custom_command scoping the easiest way to do this
# is to run the script at cmake-time.
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tfm_parse_manifest_list.py
                                  -m ${MANIFEST_LISTS}
                                  -f ${GENERATED_FILE_LISTS}
                                  -o ${CMAKE_BINARY_DIR}/generated
                                  -e ${OUT_OF_TREE_MANIFEST_LIST}
    RESULT_VARIABLE RET
)

if(NOT RET EQUAL 0)
    message(FATAL_ERROR "File generation failed")
endif()
