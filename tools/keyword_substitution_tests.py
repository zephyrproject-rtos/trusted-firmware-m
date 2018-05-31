#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

from keyword_substitution import keyword_substitute, log_print, Verbosity

try:
    import yaml
except ImportError as e:
    print e, "To install it, type:"
    print "pip install PyYAML"
    exit(1)

VERBOSITY = Verbosity.warning
log_print(Verbosity.debug, "Setting verbosity to", VERBOSITY, verbosity=VERBOSITY)

def testsuite():
    with open('keyword_substitution_test_vectors.yaml', 'r') as test_vectors_file:
        test_yaml = yaml.load(test_vectors_file)
        test_list = test_yaml["test_list"]

    for tcidx, test in enumerate(test_list):
        expected = ''.join(test["expected"])
        print
        log_print(Verbosity.debug, "template:", test["template"])
        log_print(Verbosity.debug, "db:", test["db"])
        outlist = keyword_substitute(test["db"], test["template"], "report")
        log_print(Verbosity.debug, outlist)
        outstring = ""
        for outline in outlist:
            outstring += ''.join(outline) + "\n"
        log_print(Verbosity.info, "Got:")
        log_print(Verbosity.info, outstring)
        if outstring == expected:
            print "Test", tcidx, "PASSED"
            test["result"] = "PASSED"
        else:
            print "Test", tcidx, "FAILED, expected:"
            print expected
            test["result"] = "FAILED"

    print
    print "Test summary:"
    for idx, test in enumerate(test_list):
        print "Test", idx, test["result"]

def main():
    testsuite()

if __name__ == "__main__":
    main()
