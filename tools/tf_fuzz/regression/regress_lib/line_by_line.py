#!/usr/bin/env python3
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
'''
The functions here perform straight-forward line-by-line comparisons of regression 
TF-Fuzz regression tests.  These are not sufficient to address cases where the PSA
calls are randomized.

Each given line of the expected files (exp_stdout_stderr and exp_test.c) functions as
Python regex for the corresonding line in the actual/generated corresponding file.
Actually, the expected-file lines may need a little bit of tweaking first.  There are
three scenarios:
*  The line in exp_* file contains no regex content at all.  In this case, the two
   lines must be exactly identical, character for character. The actual/generated 
   output files do not contain regexes, but can contain parentheses or other 
   characters that "look like" regex content, and thus confuse re.match().  So, it's 
   first checked for an exact string == match. 
*  The line in the exp_* file contains one or more standard-Python regex patterns to 
   match.  In this case, a Python re.match() will still report a match.
*  The line in the exp_* file contains one or more non-standard regex pattern, in
   which case that non-standard regex pattern needs to be replaced with the actual,
   expected character string.

As described in the above-cited README, these non-standard regex wildcards in the 
exp_* files take either of two formats:
*  "@@@" a 3-digit pattern number "@@@" (e.g., "@@@005@@@"):  This denotes a pattern 
   of no particular length that must match the same text every occurrence in the 
   actual/generated file.
*  "@@" a 3-digit pattern number "@" a 2-digit length in chars "@@":  Same idea as
   the previous pattern, except that it also has a specific length.
   
To address these special regex wildcards, check_gen_test() below has to:
1.  Isolate the wildcard from the rest of the string,
2.  Check that wildcard against a Python dictionary relating the wildcard name to 
    its expected-text substitution value,
3.  If not present in the dictionary, create a new dictionary entry relating the 
    wildcard text to the text found in that spot in the actual/generated file,
4.  Replace that wildcard text with the expected value from the hash, then
5.  As with all lines, perform the re.match() between the two lines.
'''

import sys, os, re, string, pdb


'''
mask_other_wildcards(), used by resolve_wildcards() below, is used in harvesting
the value of a certain wildcard of the sort described above.  After the caller
replaces the wildcard of interest with a regex to retrieve that data from actual-
output file, it passes the string into here, to replace all other wildcards with
"anything goes" regexes.
'''
def mask_other_wildcards (a_string):
    # Unsized wildcards:
    while True:
        matchInfo = re.match ('.*(@@@\d\d\d@@@)', a_string)
        if not matchInfo:
            break
        wildcard = matchInfo.group(1)
        a_string = a_string.replace (wildcard, '.*')
    # Sized wildcards:
    while True:
        matchInfo = re.match ('.*(@@\d\d\d@\d\d@@)', a_string)
        if not matchInfo:
            break
        wildcard = matchInfo.group(1)
        a_string = a_string.replace (wildcard, '.*')
    return a_string


'''
resolve_wildcards() resolves wildcards of the sort described above, in an expected
file line (exp) from a wildcard dictionary (wildcard_dict).  In particular, it
replaces them with what the wildcards are found to stand for in the actual test 
output (act).  If it encounters a wildcard it has not seen before, it adds it to 
the dictionary, based upon what's in the test.c output.  Further occurrences of that
wildcard, it pulls from the wildcard dictionary, meaning that the subsequent 
occurrences must resolve to the same text string.
'''
def resolve_wildcards (exp, act, wildcard_dict):
    # Loop through each wildcard on the line, filling them in with values from the 
    # wildcard dictionary, or filling them into the wildcard dictionary.

    #pdb.set_trace()

    while True:
        wildcard_sized = False
        matchInfo = re.match ('.*(@@@\d\d\d@@@)', exp)
        if not matchInfo:
            wildcard_sized = True
                # 0 = sized, and we'll fill in that size below, if we don't already know
            matchInfo = re.match ('.*(@@\d\d\d@\d\d@@)', exp)
            if not matchInfo:
                break
        wildcard = matchInfo.group(1)
        if wildcard in wildcard_dict:
            # Previously-encountered wildcard:
            wildcard_value = wildcard_dict[wildcard]
        else:
            # New wildcard:
            if wildcard_sized:  # find the size
                size_str = re.match ("@@\d\d\d@(\d\d)@@",wildcard).group(1)
                find_sub = exp.replace (wildcard, '(.{' + size_str + '})', 1)
            else:
                find_sub = exp.replace (wildcard, '(.*)', 1)
            find_sub = mask_other_wildcards (find_sub)
            matchInfo = re.match (find_sub, act)
            wildcard_value = matchInfo.group(1)
            wildcard_dict[wildcard] = wildcard_value
        exp = exp.replace (wildcard, wildcard_value)
    return exp

'''
check_file() checks that an actual-output test file (act_test_file) matches an 
expected-output file (exp_test_file), line by line, including resolving the wildcards
of the nature described above.
'''
def check_file (   exp_test_file, exp_test_file_name, 
                   act_test_file, act_test_file_name,
                   loud, quiet, ultra_quiet                   ):
    # This is the dictionary of wildcards, of the nature described above.
    wildcard_dict = dict()

    line_no = 0
    while True:
        exp_line = exp_test_file.readline()
        act_line = act_test_file.readline()
        # Ignore the special case where a seed was specified in generating actual,
        # but wasn't in expected file:
        if exp_line == 'Info:  random seed was not specified.\n' and act_line == '\n':
            print ("(Note:  Ignoring no-seed-specified message in expected.)")
            exp_line = exp_test_file.readline()
        line_no += 1
        if not quiet and not loud: print (".", end="")
        if not exp_line and act_line:
            message = "\nError:  More lines in file {} than in {}."
            if not ultra_quiet:
                print (message.format(act_test_file_name, exp_test_file_name))
            sys.exit(8)
        elif not act_line and exp_line:
            message = "\nError:  More lines in file {} than in {}."
            if not ultra_quiet:
                print (message.format(exp_test_file_name, act_test_file_name))
            sys.exit(9)
        # No line-count mismatch (so far);  either both reads succeeded or both failed:
        if not exp_line:
            break  # all lines read;  kick out of the loop.
        exp_line = exp_line.rstrip();  act_line = act_line.rstrip()
        if loud and not quiet:
            print ("\n" + str(line_no) + "\nExpect:  '" + exp_line + "'")
            print ("Actual:  '" + act_line + "'")
        # If the two lines match as raw strings then we're good:
        if exp_line == act_line:
            pass  # this line passes
        else:
            # Before doing a re.match() on this line pair, replace any nontraditional
            # wildcards with what they're expected to contain:
            if re.match (".*@@\d\d\d@", exp_line):  # just to save time if no wildcards
                exp_line = resolve_wildcards (exp_line, act_line, wildcard_dict)
            if (exp_line == "" and act_line != "") or not re.match (exp_line, act_line):
                message  = "\nError:  At line {} mismatch between {} and {}:"
                message1 = "    Expected:  {}"
                message2 = "         Got:  {}\n"
                if not ultra_quiet:
                    print (message.format(line_no, exp_test_file_name, act_test_file_name))
                    print (message1.format(exp_line))
                    print (message2.format(act_line))
                sys.exit(10)
    if not quiet:  print ("\nGenerated file complies with expected file.")


