#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
find_package(Python3)

############################### Manifest lists declaration #####################
list(APPEND MANIFEST_LISTS ${TFM_MANIFEST_LIST})

if (TFM_NS_REG_TEST OR TFM_S_REG_TEST)
    list(APPEND MANIFEST_LISTS ${TFM_TEST_PATH}/secure_fw/tfm_test_manifest_list.yaml)
endif()

if ("${TEST_PSA_API}" STREQUAL "IPC")
    # The manifest tool does not recognize CMake varibles. Do configure_file() first.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tfm_psa_ff_test_manifest_list.yaml
                   ${CMAKE_CURRENT_BINARY_DIR}/tfm_psa_ff_test_manifest_list.yaml)
    list(APPEND MANIFEST_LISTS ${CMAKE_CURRENT_BINARY_DIR}/tfm_psa_ff_test_manifest_list.yaml)
endif()

if (TFM_EXTRA_MANIFEST_LIST_FILES)
    list(APPEND MANIFEST_LISTS ${TFM_EXTRA_MANIFEST_LIST_FILES})
endif()

############################### File list declaration ##########################
set(GENERATED_FILE_LISTS ${CMAKE_CURRENT_SOURCE_DIR}/tfm_generated_file_list.yaml)
set(GENERATED_FILE_LISTS ${GENERATED_FILE_LISTS} ${TFM_EXTRA_GENERATED_FILE_LIST_PATH})

############################### Functions declaration ##########################
# Parses the given YAML "files" to find out all the items of the given "field"
# and put them to the "output_variable" as a list.
function(parse_field_from_yaml files field output_variable)
    set(local_variable "")
    foreach(yaml_file ${files})
        # Load the lines that refer to the key we selected
        file(STRINGS ${yaml_file} temp_variable REGEX " *\"${field}\":")
        # Take only the value of the key
        list(TRANSFORM temp_variable REPLACE " *\"${field}\": *" ";")
        # Remove all commas
        list(TRANSFORM temp_variable REPLACE "," "")
        # Remove all quote marks
        list(TRANSFORM temp_variable REPLACE "\"" "")
        list(APPEND local_variable ${temp_variable})
    endforeach()
    set(${output_variable} ${local_variable} PARENT_SCOPE)
endfunction()

############################### Dependency generation ##########################
# Get all the manifest files from manifest lists
foreach(MANIFEST_LIST ${MANIFEST_LISTS})
    if (NOT EXISTS ${MANIFEST_LIST})
        message(FATAL_ERROR "Manifest list ${MANIFEST_LIST} doesn't exist")
    endif()

    # Get the path of the manifest list
    get_filename_component(MANIFEST_LIST_PATH ${MANIFEST_LIST} DIRECTORY)

    # Get all the "manifest"
    parse_field_from_yaml(${MANIFEST_LIST} manifest MANIFESTS)

    foreach(MANIFEST ${MANIFESTS})
        # Convert to absolute paths
        if (NOT IS_ABSOLUTE ${MANIFEST})
            get_filename_component(MANIFEST "${MANIFEST_LIST_PATH}/${MANIFEST}" ABSOLUTE)
        endif()
        list(APPEND MANIFEST_FILES ${MANIFEST})
    endforeach()
endforeach()

parse_field_from_yaml("${GENERATED_FILE_LISTS}" template TEMPLATE_FILES)
# Replace relative paths with absolute paths
# Paths used in GENERATED_FILE_LISTS are all relative to TF-M root (${CMAKE_SOURCE_DIR})
list(TRANSFORM TEMPLATE_FILES REPLACE "^([^/\\][^:].*)" "${CMAKE_SOURCE_DIR}/\\1")

parse_field_from_yaml("${GENERATED_FILE_LISTS}" output OUTPUT_FILES)
# Replace relative paths with absolute paths
# Paths used in GENERATED_FILE_LISTS are all relative to TF-M root (${CMAKE_SOURCE_DIR})
list(TRANSFORM OUTPUT_FILES REPLACE "^([^/\\][^:].*)" "${CMAKE_BINARY_DIR}/generated/\\1")

############################### Generate Manifest config header ################
parse_field_from_yaml("${MANIFEST_LISTS}" conditional CONDITIONS)

# Isolation level and backend are required by the manifest tool
string(APPEND MANIFEST_CONFIG_H_CONTENT
       "#cmakedefine TFM_ISOLATION_LEVEL  @TFM_ISOLATION_LEVEL@\r\n")
string(APPEND MANIFEST_CONFIG_H_CONTENT
       "#cmakedefine CONFIG_TFM_SPM_BACKEND @CONFIG_TFM_SPM_BACKEND@\r\n")

foreach(CON ${CONDITIONS})
    # Secure Partitions may share the same conditions, perform duplication check here
    string(FIND ${MANIFEST_CONFIG_H_CONTENT} ${CON} CON_EXISTS)
    if (${CON_EXISTS} EQUAL -1) # Not found
        string(APPEND MANIFEST_CONFIG_H_CONTENT
               "#cmakedefine01 ${CON}\r\n")
    endif()
endforeach()

# Generate the config header
file(WRITE
     ${CMAKE_CURRENT_BINARY_DIR}/manifest_config.h.in
     ${MANIFEST_CONFIG_H_CONTENT})

configure_file(${CMAKE_CURRENT_BINARY_DIR}/manifest_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/manifest_config.h)

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

if (CONFIG_TFM_PARSE_MANIFEST_QUIET)
    set(PARSE_MANIFEST_QUIET_FLAG "-q")
else()
    set(PARSE_MANIFEST_QUIET_FLAG "")
endif()

set(MANIFEST_COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tfm_parse_manifest_list.py
    -m ${MANIFEST_LISTS}
    -f ${GENERATED_FILE_LISTS}
    -c ${CMAKE_CURRENT_BINARY_DIR}/manifest_config.h
    -o ${CMAKE_BINARY_DIR}/generated
    ${PARSE_MANIFEST_QUIET_FLAG})

add_custom_command(OUTPUT ${OUTPUT_FILES}
    COMMAND ${MANIFEST_COMMAND}
    DEPENDS ${TEMPLATE_FILES} ${MANIFEST_FILES}
    DEPENDS ${MANIFEST_LISTS} ${GENERATED_FILE_LISTS}
)

# The files need to be generated before cmake will allow them to be used as
# sources. Due to issue with custom_command scoping the easiest way to do this
# is to run the script at cmake-time.
execute_process(
    COMMAND ${MANIFEST_COMMAND}
    RESULT_VARIABLE RET
)

if(RET EQUAL 0)
    include(${CMAKE_BINARY_DIR}/generated/tools/config_impl.cmake)
else()
    message(FATAL_ERROR "Manifest tool failed to generate files!")
endif()
