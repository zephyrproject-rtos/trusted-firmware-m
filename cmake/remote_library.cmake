#-------------------------------------------------------------------------------
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

find_package(Git)

# This function applies patches if they are not applied yet.
# It assumes that patches have not been applied if it's not possible to revert them.
#
# WORKING_DIRECTORY - working directory where patches should be applied.
# PATCH_FILES - list of patches. Patches will be applied in alphabetical order.
function(apply_patches WORKING_DIRECTORY PATCH_FILES)
    # Sort list of patches in alphabetical order
    # So, you can use number prefix to order patches
    list(SORT PATCH_FILES)

    # Validate if patches are already applied
    set(EXECUTE_COMMAND "${GIT_EXECUTABLE}" apply --check -R ${PATCH_FILES})
    execute_process(COMMAND ${EXECUTE_COMMAND}
        WORKING_DIRECTORY ${WORKING_DIRECTORY}
        RESULT_VARIABLE ARE_PATCHES_APPLIED
        ERROR_QUIET
    )

    if (NOT ARE_PATCHES_APPLIED EQUAL 0)
        # Git failed to unapply the patches, so we assume that
        # patches should be applied
        set(EXECUTE_COMMAND "${GIT_EXECUTABLE}" apply --verbose ${PATCH_FILES})
        execute_process(COMMAND ${EXECUTE_COMMAND}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            RESULT_VARIABLE ARE_PATCHES_APPLIED
            COMMAND_ECHO STDOUT
        )
        if (NOT ARE_PATCHES_APPLIED EQUAL 0)
            message( FATAL_ERROR "Failed to apply patches at ${WORKING_DIRECTORY}" )
        endif()
    endif()
endfunction()


# Returns a repository URL and a reference to the commit used to checkout the repository.
#
# REPO_URL_VAR - name of variable which receives repository URL.
# TAG_VAR - name of variable which receives reference to commit.
function(_get_fetch_remote_properties REPO_URL_VAR TAG_VAR)
    # Parse arguments
    set(options "")
    set(oneValueArgs GIT_REPOSITORY GIT_TAG)
    set(multiValueArgs "")
    cmake_parse_arguments(PARSE_ARGV 2 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if (ARG_GIT_REPOSITORY)
        set(${REPO_URL_VAR} ${ARG_GIT_REPOSITORY} PARENT_SCOPE)
        set(${TAG_VAR} ${ARG_GIT_TAG} PARENT_SCOPE)
    endif()
endfunction()


# This function helps to handle options with an empty string values.
# There is a feature/bug in CMake that result in problem with the empty string arguments.
# See https://gitlab.kitware.com/cmake/cmake/-/issues/16341 for details
#
# Arguments:
#   [in]  KEY              - option name
#   [out]  KEY_VAR         - name of variable that is set to ${KEY} on exit if value is not
#                            an empty string otherwise to the empty string.
#   [out]  VALUE_VAR       - name of variable that is set to option value for ${KEY}.
#   [in/out]  ARG_LIST_VAR - name of variable that holds list of key/value pairs - arguments.
#                            Function looks for key/value pair specified by ${KEY} variable in
#                            this list. Function removes key/value pair specified by ${KEY} on
#                            exit.
#
# Example #1:
#   # We have following key/options:
#   #  GIT_SUBMODULES  ""
#   #  BOO  "abc"
#   #  HEY  "hi"
#   set(ARGS    GIT_SUBMODULES "" BOO "abc" HEY "hi")
#   # Extract key/value for option "GIT_SUBMODULES"
#   extract_key_value(GIT_SUBMODULES GIT_SUBMODULES_VAR GIT_SUBMODULES_VALUE_VAR ARGS)
#   # ${GIT_SUBMODULES_VAR} is equal to ""
#   # ${GIT_SUBMODULES_VALUE_VAR} is equal to ""
#
# Example #2:
#   # We have following key/options:
#   #  GIT_SUBMODULES  "name"
#   #  BOO  "abc"
#   #  HEY  "hi"
#   set(ARGS    GIT_SUBMODULES "name" BOO "abc" HEY "hi")
#   # Extract key/value for option "GIT_SUBMODULES"
#   extract_key_value(GIT_SUBMODULES GIT_SUBMODULES_VAR GIT_SUBMODULES_VALUE_VAR ARGS)
#   # ${GIT_SUBMODULES_VAR} is equal to "GIT_SUBMODULES"
#   # ${GIT_SUBMODULES_VALUE_VAR} is equal to "name"
function(extract_key_value KEY KEY_VAR VALUE_VAR ARG_LIST_VAR)
    list(FIND ${ARG_LIST_VAR} ${KEY} KEY_INDEX)
    if(${KEY_INDEX} GREATER_EQUAL 0)
        # Variable has been set, remove KEY
        list(REMOVE_AT ${ARG_LIST_VAR} ${KEY_INDEX})

        # Validate that there is an option value in the list of arguments
        list(LENGTH ${ARG_LIST_VAR} ARG_LIST_LENGTH)
        if(${KEY_INDEX} GREATER_EQUAL ${ARG_LIST_LENGTH})
            message(FATAL_ERROR "Missing option value for ${KEY}")
        endif()

        # Get value
        list(GET ${ARG_LIST_VAR} ${KEY_INDEX} VALUE)

        # Remove value in the list
        list(REMOVE_AT ${ARG_LIST_VAR} ${KEY_INDEX})

        # Update argument list
        set(${ARG_LIST_VAR} ${${ARG_LIST_VAR}} PARENT_SCOPE)

        # Set KEY_VAR & VALUE_VAR
        set(${KEY_VAR} ${KEY} PARENT_SCOPE)
        set(${VALUE_VAR} ${VALUE} PARENT_SCOPE)
    else()
        # Variable is not defined, set KEY_VAR & VALUE_VAR to empty strings
        set(${KEY_VAR} "" PARENT_SCOPE)
        set(${VALUE_VAR} "" PARENT_SCOPE)
    endif()
endfunction()


# This function allows to fetch library from a remote repository or use a local
# library copy.
#
# You can specify location of directory with patches. Patches are applied in
# alphabetical order.
#
# Arguments:
# [in]     LIB_NAME <name> - library name
# [in/out] LIB_SOURCE_PATH_VAR <var> - name of variable which holds path to library source
#           or "DOWNLOAD" if sources should be fetched from the remote repository. This
#           variable is updated in case if library is downloaded. It will point
#           to the path where FetchContent_Populate will locate local library copy.
# [out]    LIB_BINARY_PATH_VAR <var> - optional name of variable which is updated to
#           directory intended for use as a corresponding build directory if
#           library is fetched from the remote repository.
# [in]     LIB_BASE_DIR <path>  - is used to set FETCHCONTENT_BASE_DIR.
# [in]     LIB_PATCH_DIR <path> - optional path to local folder which contains patches
#           that should be applied.
# [in]     GIT_REPOSITORY, GIT_TAG, ... - see https://cmake.org/cmake/help/latest/module/ExternalProject.html
#           for more details
#
# This function set CMP0097 to NEW if CMAKE_VERSION is greater or equal than 3.18.0.
# Because of https://gitlab.kitware.com/cmake/cmake/-/issues/20579 CMP0097 is
# non-functional until cmake 3.18.0.
# See https://cmake.org/cmake/help/latest/policy/CMP0097.html for more info.
function(fetch_remote_library)
    # Parse arguments
    set(options "")
    set(oneValueArgs LIB_NAME LIB_SOURCE_PATH_VAR LIB_BINARY_PATH_VAR LIB_BASE_DIR LIB_PATCH_DIR)
    set(multiValueArgs FETCH_CONTENT_ARGS)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if(ARG_LIB_BASE_DIR)
        set(FETCHCONTENT_BASE_DIR "${ARG_LIB_BASE_DIR}")
    endif()

    # Set to not download submodules if that option is available
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
        cmake_policy(SET CMP0097 NEW)
    endif()

    if ("${${ARG_LIB_SOURCE_PATH_VAR}}" STREQUAL "DOWNLOAD")
        # Process arguments which can be an empty string
        # There is a feature/bug in CMake that result in problem with empty string arguments
        # See https://gitlab.kitware.com/cmake/cmake/-/issues/16341 for details
        extract_key_value(GIT_SUBMODULES GIT_SUBMODULES GIT_SUBMODULES_VALUE ARG_FETCH_CONTENT_ARGS)

        # Validate that there is no empty arguments to FetchContent_Declare
        LIST(FIND ARG_FETCH_CONTENT_ARGS "" EMPTY_VALUE_INDEX)
        if(${EMPTY_VALUE_INDEX} GREATER_EQUAL 0)
            # There is an unsupported empty string argument, FATAL ERROR!
            math(EXPR EMPTY_KEY_INDEX "${EMPTY_VALUE_INDEX} - 1")
            list(GET ARG_FETCH_CONTENT_ARGS ${EMPTY_KEY_INDEX} EMPTY_KEY)
            # TODO: Use extract_key_value if you have argument with empty value (see GIT_SUBMODULES above)
            message(FATAL_ERROR "fetch_remote_library: Unexpected empty string value for ${EMPTY_KEY}. "
                                "Please, validate arguments or update fetch_remote_library to support empty value for ${EMPTY_KEY}!!!")
        endif()

        # Content fetching
        FetchContent_Declare(${ARG_LIB_NAME}
            ${ARG_FETCH_CONTENT_ARGS}
            "${GIT_SUBMODULES}"      "${GIT_SUBMODULES_VALUE}"
        )

        FetchContent_GetProperties(${ARG_LIB_NAME})
        if(NOT ${ARG_LIB_NAME}_POPULATED)
            FetchContent_Populate(${ARG_LIB_NAME})

            # Get remote properties
            _get_fetch_remote_properties(REPO_URL_VAR TAG_VAR ${ARG_FETCH_CONTENT_ARGS})
            set(${ARG_LIB_SOURCE_PATH_VAR} ${${ARG_LIB_NAME}_SOURCE_DIR} CACHE PATH "Library has been downloaded from \"${REPO_URL_VAR}\", tag \"${TAG_VAR}\"" FORCE)
            if (DEFINED ARG_LIB_BINARY_PATH_VAR)
                set(${ARG_LIB_BINARY_PATH_VAR} ${${ARG_LIB_NAME}_BINARY_DIR} CACHE PATH "Path to build directory of \"${ARG_LIB_NAME}\"")
            endif()
        endif()
    endif()

    if (ARG_LIB_PATCH_DIR)
        # look for patch files
        file(GLOB PATCH_FILES "${ARG_LIB_PATCH_DIR}/*.patch")

        if(PATCH_FILES)
            # Apply patches for existing sources
            apply_patches("${${ARG_LIB_SOURCE_PATH_VAR}}" "${PATCH_FILES}")
        endif()
    endif()
endfunction()
