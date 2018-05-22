#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import os
from keyword_substitution import keyword_substitute, Verbosity, log_print

try:
    import yaml
except ImportError as e:
    print e, "To install it, type:"
    print "pip install PyYAML"
    exit(1)

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

# Initialisation of globals
part_db_basename = 'tfm_partition_list.inc'
part_defs_basename = 'tfm_partition_defs.inc'
sfid_map_basename = 'tfm_sfid_list.inc'

# Functions
def load_manifest_list(file):
    db = []
    manifest_list = yaml.load(file)
    for item in manifest_list["manifest_list"]:
        manifest_path = os.path.join('tools', item['manifest'])
        try:
            file = open(manifest_path)
            manifest = yaml.load(file)
            db.append({"manifest": manifest, "attr": item})
        except IOError:
            print "Manifest for "+item['name']+" cannot be opened at path "+item['manifest']
    return db
# def load_yaml_file

def generate(db, path, basename):
    outfile = \
        open(os.path.join(path, basename), "w")
    with open(os.path.join(path, basename+'.template'), "r") as template_file:
        template = template_file.readlines()

    output = []
    blocks = []
    blocklines = []
    MISSING_KEYS_ACTION = 'replace False'
    inblock = False
    linecnt = len(template)
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
    log_print(Verbosity.info, "Blocks:" + str(blocks))
    outfile.close()
# def generate()

# main
def main():
    with open(os.path.join('tools', 'manifest_list.yaml')) \
                                                        as manifest_list_yaml_file:
        # Read manifest list file, build database
        db = load_manifest_list(manifest_list_yaml_file)

    path = os.path.join('secure_fw', 'services')
    generate(db, path, part_db_basename)
    generate(db, path, sfid_map_basename)
    generate(db, path, part_defs_basename)

if __name__ == "__main__":
    main()
