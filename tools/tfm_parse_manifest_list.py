#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import os
from keyword_substitution import Verbosity, log_print
from generate_from_template import generate_from_template_file

try:
    import yaml
except ImportError as e:
    print e, "To install it, type:"
    print "pip install PyYAML"
    exit(1)

VERBOSITY = Verbosity.warning
log_print(Verbosity.debug, "Setting verbosity to", VERBOSITY, verbosity=VERBOSITY)

# All operations assume tf-m repo root as active working directory

# Functions
def load_manifest_list(file):
    db = []
    manifest_list = yaml.load(file)
    for item in manifest_list["manifest_list"]:
        manifest_path = item['manifest']
        try:
            file = open(manifest_path)
            manifest = yaml.load(file)
            db.append({"manifest": manifest, "attr": item})
        except IOError:
            print "Manifest for "+item['name']+" cannot be opened at path "+item['manifest']
    return db
# def load_manifest_list

# main
def main():
    with open(os.path.join('tools', 'tfm_manifest_list.yaml')) \
                                                    as manifest_list_yaml_file:
        # Read manifest list file, build database
        db = load_manifest_list(manifest_list_yaml_file)

    with open(os.path.join('tools', 'tfm_generated_file_list.yaml')) \
                                                    as file_list_yaml_file:
        # read list of files that need to be generated from templates using db
        file_list_yaml = yaml.load(file_list_yaml_file)
        file_list = file_list_yaml["file_list"]
    generate_from_template_file(db, file_list)
    print "Generation of files done"

if __name__ == "__main__":
    main()
