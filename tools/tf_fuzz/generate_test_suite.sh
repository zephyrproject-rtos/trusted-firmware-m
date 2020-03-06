#!/usr/bin/env bash
#
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

set -e

SUITE_DIR_NAME=tfm_fuzz

show_help()
{
    echo "Usage: $0 <template_dir> <suites_dir> "
    echo ""
    echo "Where: "
    echo "    template_dir: The directory containing template files for the"
    echo "                  fuzzing tool"
    echo "    suites_dir:   The directory to generate the test suite to."
}

if [ $# != 2 ]
then
    echo "Invalid number of parameters."
    show_help
    exit 1
fi

INPUT_DIR=$1
SUITES_DIR=$2

# Check that the input directory exists
if [ ! -d "$INPUT_DIR" ]
then
    echo "Template directory '$INPUT_DIR' doesn't exist"
    exit 1
fi

# Check that the input directory contains files
if [ `ls "$INPUT_DIR"| wc -l` == 0 ]
then
    echo "Template directory '$INPUT_DIR' doesn't contain files"
    exit 1
fi

if [ ! -d "$SUITES_DIR" ]
then
    echo "Suites directory '$SUITES_DIR' doesn't exist"
    exit 1
fi

# Check whether a fuzz test suite was generated before
if [ -d "$SUITES_DIR/$SUITE_DIR_NAME" ]
then
    echo "'$SUITE_DIR_NAME' exists."
    echo "A TF fuzz test suite is already generated, please delete it before running this script"
    exit 1
fi

#get absolute path of the suit dir TF_FUZZ
pushd $SUITES_DIR > /dev/null
SUITE_ABSOLUTE_DIR=`pwd`/$SUITE_DIR_NAME
popd

# creating directory for the fuzz tests tool
echo "Creating '$SUITES_DIR/$SUITE_DIR_NAME/non_secure'"
mkdir -p $SUITES_DIR/$SUITE_DIR_NAME/non_secure

# generate additional files for the suite:
CMAKELIST=$SUITES_DIR/$SUITE_DIR_NAME/CMakeLists.txt
TESTSUITE=$SUITES_DIR/$SUITE_DIR_NAME/non_secure/tf_fuzz_testsuite.c
TESTSUITE_HEADER=$SUITES_DIR/$SUITE_DIR_NAME/non_secure/tf_fuzz_testsuite.h

# generate data for the testcases
# Iterate over the testcase files and
# - Run the fuzzer on the test template
# - Generate a test function name, and replace the generic test_thread to that
#   in the generated c file
# - append the generated c file to the CmakeList file.
declare -A FILENAMES
declare -A FUNC_NAMES
declare -A PURPOSES
for f in `ls $INPUT_DIR`
do
    FILE="$SUITES_DIR/$SUITE_DIR_NAME/non_secure/$f.c"
    FILENAMES[$f]=$FILE

    SEED=$RANDOM
    echo "Generating testcase $f with seed $SEED"
    ./tfz -z $INPUT_DIR/$f $FILE $SEED

    echo "in File ${FILENAMES[$f]}"

    PURPOSE=`grep -A 1 'Test purpose' $FILE | tail -n 1 | cut -d '*' -f 2 | sed -e 's/^[[:space:]]*//'`
    PURPOSES[$f]=$PURPOSE

    FUNC_NAME="$PURPOSE"
    FUNC_NAME=`echo "$FUNC_NAME" | \
               sed 's/^\s*//g; s/\s*$//g; s/\s\s*/_/g'`
    FUNC_NAME=`echo "$FUNC_NAME" | \
               sed 's/\-/_/g; s/"//g; s/\\\//g'`
    FUNC_NAME=`echo "$FUNC_NAME" | \
               sed "s/#//g; s/'//g; s/,//g; s/(//g; s/)//g"`
    FUNC_NAMES[$f]=$FUNC_NAME

    echo "    Changing 'test_thread' to '${FUNC_NAMES[$f]}'"

    sed -i "s/test_thread/$FUNC_NAME/g" $FILE

done

# generate the non-secure testsuite file
echo '#include <stdio.h>' >> $TESTSUITE
echo '#include <string.h>' >> $TESTSUITE
echo '' >> $TESTSUITE
echo '#include "tfm_api.h"' >> $TESTSUITE
echo '#include "psa_manifest/sid.h"' >> $TESTSUITE
echo '#include "test_framework.h"' >> $TESTSUITE
echo '' >> $TESTSUITE
echo '/* Forward declaring static test functions */' >> $TESTSUITE
for f in `ls $INPUT_DIR`
do
    echo "static void test_${FUNC_NAMES[$f]}(struct test_result_t *ret);" >> $TESTSUITE
done
echo '/* Forward declaring functions */' >> $TESTSUITE
for f in `ls $INPUT_DIR`
do
    echo "void ${FUNC_NAMES[$f]}(struct test_result_t *ret);" >> $TESTSUITE
done
echo '' >> $TESTSUITE
echo 'static struct test_t tf_fuzz_tests[] = {' >> $TESTSUITE
for f in `ls $INPUT_DIR`
do
    echo "    {&test_${FUNC_NAMES[$f]}, \"${FUNC_NAMES[$f]}\", \"${PURPOSES[$f]}\", {0} }," >> $TESTSUITE
done
echo '}; ' >> $TESTSUITE
echo '' >> $TESTSUITE
echo 'void register_testsuite_tf_fuzz_test(struct test_suite_t *p_test_suite)' >> $TESTSUITE
echo '{' >> $TESTSUITE
echo '    uint32_t list_size;' >> $TESTSUITE
echo '' >> $TESTSUITE
echo '    list_size = (sizeof(tf_fuzz_tests) / sizeof(tf_fuzz_tests[0]));' >> $TESTSUITE
echo '' >> $TESTSUITE
echo '    set_testsuite("TF-M fuzz tests (TF_FUZZ_TEST)",' >> $TESTSUITE
echo '                  tf_fuzz_tests, list_size, p_test_suite);' >> $TESTSUITE
echo '}' >> $TESTSUITE
echo '' >> $TESTSUITE
for f in `ls $INPUT_DIR`
do
    echo "static void test_${FUNC_NAMES[$f]}(struct test_result_t *ret)" >> $TESTSUITE
    echo '{' >> $TESTSUITE
    echo "    ${FUNC_NAMES[$f]}(ret);" >> $TESTSUITE
    echo '    if (ret->val != TEST_PASSED) {' >> $TESTSUITE
    echo '        return;' >> $TESTSUITE
    echo '    }' >> $TESTSUITE
    echo '' >> $TESTSUITE
    echo '}' >> $TESTSUITE
done

# generate the CmakeList file
echo "cmake_policy(SET CMP0079 NEW)" >> $CMAKELIST
echo "" >> $CMAKELIST
echo "add_library(tfm_test_suite_tf_fuzz_ns STATIC EXCLUDE_FROM_ALL)" >> $CMAKELIST
echo "" >> $CMAKELIST
echo "target_sources(tfm_test_suite_tf_fuzz_ns" >> $CMAKELIST
echo "    PRIVATE" >> $CMAKELIST
for f in `ls $INPUT_DIR`
do
    echo "        $SUITE_ABSOLUTE_DIR/non_secure/$f.c" >> $CMAKELIST
done
echo "        $SUITE_ABSOLUTE_DIR/non_secure/tf_fuzz_testsuite.c" >> $CMAKELIST
echo ")" >> $CMAKELIST
echo "" >> $CMAKELIST
echo "target_include_directories(tfm_test_suite_tf_fuzz_ns" >> $CMAKELIST
echo "    PUBLIC" >> $CMAKELIST
echo "        ./non_secure" >> $CMAKELIST
echo "    PRIVATE" >> $CMAKELIST
echo "        ." >> $CMAKELIST
echo ")" >> $CMAKELIST
echo "" >> $CMAKELIST
echo "target_link_libraries(tfm_test_suite_tf_fuzz_ns" >> $CMAKELIST
echo "    PRIVATE" >> $CMAKELIST
echo "        tfm_test_framework_ns" >> $CMAKELIST
echo "        tfm_test_suite_its_ns" >> $CMAKELIST
echo ")" >> $CMAKELIST
echo "" >> $CMAKELIST
echo "target_link_libraries(tfm_ns_tests" >> $CMAKELIST
echo "    INTERFACE" >> $CMAKELIST
echo "        tfm_test_suite_tf_fuzz_ns" >> $CMAKELIST
echo ")" >> $CMAKELIST

# generate the testsuite header file
echo '#ifndef __PSA_API_FUZZ_TESTSUITE_H__' >> $TESTSUITE_HEADER
echo '#define __PSA_API_FUZZ_TESTSUITE_H__' >> $TESTSUITE_HEADER
echo '' >> $TESTSUITE_HEADER
echo 'void register_testsuite_tf_fuzz_test(struct test_suite_t *p_test_suite);' >> $TESTSUITE_HEADER
echo '' >> $TESTSUITE_HEADER
echo '#endif /* __PSA_API_FUZZ_TESTSUITE_H__ */' >> $TESTSUITE_HEADER

# print some instruction on the screen
echo ''
echo ''
echo '========================================================================='
echo '= The test suite generation is done. It can be compiled to TF-M by'
echo '= providing the following options to the CMake generate command:'
echo '='
echo '=     -DTFM_FUZZER_TOOL_TESTS=1'
echo '=     -DTFM_FUZZER_TOOL_TESTS_CMAKE_INC_PATH='$SUITE_ABSOLUTE_DIR
echo '========================================================================='
