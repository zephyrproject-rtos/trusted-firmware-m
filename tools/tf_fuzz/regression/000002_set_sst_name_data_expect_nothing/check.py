#!/usr/bin/env python3
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

'''
    Please read .../tf_fuzz/regression/README to understand this code.
    
    Please also read the comments in .../tf_fuzz/regression/regress_lib/line_by_line.py.
'''

import sys, os, re, string, pdb
sys.path.append ("../regress_lib")
import line_by_line

        
# Describe script usage:
def usage():
    print ('''
    Command-line parameters:
    1.  The test-template file, nominally named "template",
    2.  The expected combined stdout/stderr output, nominally named "exp_stdout_stderr",
    3.  The actual/generated combined stdout/stderr output, nominally named "stdout_stderr",
    4.  The expected .c test file, nominally named "exp_test.c", and
    5.  The actual, generated .c test file, nominally named "test.c".
    
    Optionally, *before* these five arguments, you may add switches thus:
    "--v" for verbose mode, to view line-by-line comparisons actual vs. expected,
    "--q" to only print error messages and a successful-completion message,
    "--qq" same as --q but not even printing out the completion message, and
    "--s 12345" to run TF-Fuzz with seed value 12345 (or whatever).
    ''')


def main():
    # See if we're supposed to be quiet:
    loud = quiet = ultra_quiet = False
    seed = ""
    while sys.argv[1][0] == "-":
        if sys.argv[1] == "--v":
            loud = True
            sys.argv.pop(1)
        elif sys.argv[1] == "--q":
            quiet = True
            sys.argv.pop(1)
        elif sys.argv[1] == "--qq":
            quiet = ultra_quiet = True
            sys.argv.pop(1)
        elif sys.argv[1] == "--s":
            sys.argv.pop(1)
            seed = sys.argv[1]
            sys.argv.pop(1)
            if not seed.isnumeric():
                print ('The --s seed argument was not a number.')
                usage()
                sys.exit(1)
        
    # Run TF-Fuzz:
    if not quiet: print ("Running TF-Fuzz...  ")
    os.system ('rm -f stdout_stderr test.c')
    command = '../../tfz -v ./' + sys.argv[1] + ' ./' + sys.argv[5]
    command += ' ' + seed + ' >' + sys.argv[3] + ' 2>&1'
    if loud:
        print (command)
    if os.system (command) == 0:
        if not quiet: print ("TF-Fuzz run complete.")
    else:
        print ('Could not run TF-Fuzz;  please see stdout_stderr file.')
        sys.exit(2)
    
    # Attempt to open files indicated on command line:
    if len(sys.argv) != 6:
        message = '{} requires 5 command-line arguments. Exiting.'
        print (message.format(sys.argv[0]), file=sys.stderr)
        usage()
        sys.exit(3)
    template_file_name =  sys.argv[1]
    exp_stdout_file_name = sys.argv[2]
    act_stdout_file_name = sys.argv[3]
    exp_test_file_name = sys.argv[4]
    act_test_file_name = sys.argv[5]

    try:
        template_file =  open (template_file_name, 'rt')
        exp_stdout_file = open (exp_stdout_file_name, 'rt')
        act_stdout_file = open (act_stdout_file_name, 'rt')
        exp_test_file = open (exp_test_file_name, 'rt')
        act_test_file = open (act_test_file_name, 'rt')
    except FileNotFoundError:
        print ('One or more files could not be found.')
        usage();
        sys.exit(4)
    except:
        print ('Something went wrong trying to open the input files.')
        usage();
        sys.exit(5)
    else:
        message =  '\nInput files:\n    {},\n    {},\n    {},\n'
        message += '    {}, and\n    {}\nopened successfully.\n'
        if not quiet:
            print (message.format (template_file_name, exp_stdout_file_name, 
                   act_stdout_file_name, exp_test_file_name, act_test_file_name))
    
    # Check it all:
    if not quiet: print ("Checking stdout and stderr:  ", end="")
    line_by_line.check_file (   exp_stdout_file, exp_stdout_file_name, 
                                act_stdout_file, act_stdout_file_name,
                                loud, quiet, ultra_quiet                      )
    if not quiet: print ("\nChecking test C file:  ", end="")
    line_by_line.check_file (   exp_test_file, exp_test_file_name, 
                                act_test_file, act_test_file_name,
                                loud, quiet, ultra_quiet                      )
    
    # Ran to completion normally, so pass:
    if not ultra_quiet: print ("Regression test passed.")
    sys.exit(0)

if __name__ == "__main__": main()
