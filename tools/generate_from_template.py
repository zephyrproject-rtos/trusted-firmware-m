#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import os
from keyword_substitution import keyword_substitute, Verbosity, log_print

VERBOSITY = Verbosity.warning
log_print(Verbosity.debug, "Setting verbosity to", VERBOSITY, verbosity=VERBOSITY)

controlsymbol = "@!GENERATOR"
controlblockstart = "@!GENERATOR_BLOCK_START!@"
controlblockend = "@!GENERATOR_BLOCK_END!@"
controlconditionstart = "@!GENERATOR_CONDITIONAL_START!@"
controlconditionelse = "@!GENERATOR_CONDITIONAL_ELSE!@"
controlconditionend = "@!GENERATOR_CONDITIONAL_END!@"
control_print_iteration_counter = "@!GENERATOR_ITERATION_COUNTER!@"
control_print_donotedit_warning = "@!GENERATOR_DONOTEDIT_WARNING!@"

donotedit_warning = "/*********** " + \
                    "WARNING: This is an auto-generated file. Do not edit!" + \
                    " ***********/\n"

# All operations assume tf-m repo root as active working directory

# Functions
def generate(db, outfile_name):
    outfile = \
        open(outfile_name, "w")
    with open(outfile_name + '.template', "r") as template_file:
        template = template_file.readlines()

    output = []
    blocks = []
    blocklines = []
    MISSING_KEYS_ACTION = 'replace False'
    inblock = False
    iteration_counter = 0
    for lineno, line in enumerate(template):
        if controlblockstart in line:
            inblock = True
            log_print(Verbosity.info, "Blockstart:", str(lineno))
            blocklines = []
        elif controlblockend in line:
            inblock = False
            iteration_counter = 0
            log_print(Verbosity.info, "Blocklines:", str(blocklines))
            for manifest in db:
                print_blocked = False
                for line in blocklines:
                    outlist = keyword_substitute(manifest, line, MISSING_KEYS_ACTION)
                    outstring = ""
                    for outline in outlist:
                        outstring += ''.join(outline)
                    log_print(Verbosity.info, outstring)
                    if controlconditionstart in outstring:
                        if 'False' in outstring:
                            log_print(Verbosity.info, "PRINT BLOCKED")
                            print_blocked = True
                    elif controlconditionend in outstring:
                        log_print(Verbosity.info, "PRINT ENABLED")
                        print_blocked = False
                    elif controlconditionelse in outstring:
                        log_print(Verbosity.info, "PRINT " + str(print_blocked))
                        print_blocked = not print_blocked
                    else:
                        if control_print_iteration_counter in outstring:
                            outstring = outstring.replace(
                                            control_print_iteration_counter,
                                            str(iteration_counter))
                        elif controlsymbol in outstring:
                            print "Invalid control symbol:", outstring
                            print "exiting"
                            exit(1)
                        if not print_blocked:
                            outfile.write(outstring)
                iteration_counter += 1
                # end for manifest in db
            blocks.append(blocklines)
        elif inblock:
            # inside a generator block
            # append line to blocklines to be processed by template generator
            blocklines.append(line)
        else:
            # outside a generator block
            if control_print_donotedit_warning in line:
                # print do not edit warning - Note: ignore rest of input line
                line = donotedit_warning
            elif control_print_iteration_counter in line:
                # line contains an iteration counter request, replace with value
                line = line.replace(control_print_iteration_counter, str(iteration_counter))
            elif controlsymbol in line:
                print "Invalid control symbol:", line
                print "exiting"
                exit(1)
            outfile.write(line)
    log_print(Verbosity.debug, "Blocks:" + str(blocks))
    outfile.close()
# def generate()

def generate_from_template_file(db, file_list):
    for file in file_list:
        outfile = file["output"]
        generate(db, outfile)
