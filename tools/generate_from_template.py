#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

from __future__ import unicode_literals
import os, re, io
from keyword_substitution import keyword_substitute, Verbosity, log_print, REkeychain

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
def substitute(manifest, line, MISSING_KEYS_ACTION):
    outlist = keyword_substitute(manifest, line, MISSING_KEYS_ACTION)
    outstring = ""
    for outline in outlist:
        outstring += ''.join(outline)
    log_print(Verbosity.info, outstring)
    return outstring

def generate(db, outfile_name):
    outfile = io.open(outfile_name, "w", newline='\n')
    with io.open(outfile_name + '.template', "r") as template_file:
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
                ignore_line = False
                for line in blocklines:
                    if controlconditionstart in line:
                        outstring = substitute(manifest, line, MISSING_KEYS_ACTION)
                        if 'False' in outstring:
                            log_print(Verbosity.info, "PRINT BLOCKED")
                            ignore_line = True
                    elif controlconditionend in line:
                        log_print(Verbosity.info, "PRINT ENABLED")
                        ignore_line = False
                    elif controlconditionelse in line:
                        log_print(Verbosity.info, "PRINT " + str(ignore_line))
                        ignore_line = not ignore_line
                    elif not ignore_line:
                        if re.search(REkeychain, line):
                            outstring = substitute(manifest, line, MISSING_KEYS_ACTION)
                        else:
                            outstring = line
                        if control_print_iteration_counter in outstring:
                            outstring = outstring.replace(
                                            control_print_iteration_counter,
                                            str(iteration_counter))
                        elif controlsymbol in outstring:
                            print "Invalid control symbol:", outstring
                            print "exiting"
                            exit(1)
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
        print "Generating", outfile
        generate(db, outfile)
