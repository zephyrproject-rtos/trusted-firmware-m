#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import datetime
import os
import types
import yaml
from string import Template

# Set default values
py_app_path = os.path.abspath(__file__).split(os.path.basename(__file__))[0]
py_app_version = "1.0"
default_yaml_file = "tfm_sst_policy_db_definition.yaml"
default_output_path = "secure_fw/services/secure_storage/assets"
default_output_filename = "sst_asset_defs"

def write_asset_file(file_type, asset_tpl_data, output_filename):
    """Writes asset definition file.

    file_type       -- Type of file to write (header or source)
    asset_tpl_data  -- Assets data to populate the template and write in the
                       file
    output_filename -- Filename of the output files <output_filename>.c/h
    """

    # Select template and set output file extension
    if file_type == "header":
        tpl_filename = "sst_asset_defs.h.template"
        output_filename += ".h"
    else:
        tpl_filename = "sst_asset_defs.c.template"
        output_filename += ".c"

    # Read template from file
    fd = open(py_app_path + tpl_filename, 'r')

    file_template = Template(fd.read().replace("(c) 2018","(c) ${gen_year}"))

    # Open output file
    fd = open(output_filename, 'w')

    # Add year in which the file is generated
    gen_year = datetime.datetime.now().year

    if default_output_path and default_output_filename in output_filename:
        gen_year = "2017-" + str(gen_year)

    asset_tpl_data['gen_year'] = gen_year

    auto_gen_notice_str = ("/**********************************************/\n"
                           "/* Automatically-generated file. Do not edit! */\n"
                           "/**********************************************/")

    asset_tpl_data['auto_gen_notice'] = auto_gen_notice_str

    # Write content in output file
    fd.write(file_template.substitute(asset_tpl_data))


def validate_asset_fields(asset, field_list, asset_name=""):
    """Validates if a set of fields exist in an asset definition, and the fields
       types are correct.

    asset      -- Asset definitions with the fields to check
    field_list -- List of fields which must be present in the asset definition
    asset_name -- Asset name to be used in error messages only
    """

    # Define error message templates
    tpl_missing_field = "[Error] Missing \"{}\" field in asset {}"
    tpl_bad_type = ("[Error] Incorrect \"{}\" field type in asset {}. "
                    "Current type {}, correct type {}")

    if asset_name == "" and 'name' in asset:
        asset_name = asset['name']

    for field in field_list:
        if field['fname'] not in asset:
            raise Exception(tpl_missing_field.format(field['fname'],
                                                     asset_name))
        if not isinstance(asset[field['fname']], field['ptype']):
            raise Exception(tpl_bad_type.format(field['fname'],
                                                asset_name,
                                                type(asset[field['fname']]),
                                                field['ptype']))

def validate_asset_perms_fields(asset_perms_list, asset_name):
    """Validates asset permissions from YAML file.

    asset_perms_list -- List of asset permissions to validate
    asset_name       -- Asset name to be used in error messages only
    """

    # Define list of available asset permissions
    asset_perms = ["REFERENCE","READ","WRITE"]

    # Define error message templates
    tpl_bad_client = ("[Error] The asset {} has a bad \"{}\". "
                      "It must be different from 0. The current value is {}")
    tpl_bad_perm = ("[Error] The asset {} has invalid permissions for client ID"
                    " {}. Only {} perms can be used; got {}")

    # Set of fields and types which define the client permission over an asset
    asset_perms_fields = [{'fname': "client_id", 'ptype': int},
                          {'fname': "perms",     'ptype': list}]

    for client in asset_perms_list:
        validate_asset_fields(client, asset_perms_fields, asset_name)

        if client['client_id'] == 0:
            raise Exception(tpl_bad_client.format(asset_name,
                                                  "client_id",
                                                  client['client_id']))

        for flag in client['perms']:
            if flag not in asset_perms:
                raise Exception(tpl_bad_perm.format(asset_name,
                                                    client['client_id'],
                                                    perm_perms,
                                                    flag))

def validate_asset_defs(asset_defs):
    """Validates asset definitions from YAML file.

    asset_defs -- Asset definitions to validate
    """

    # Define error message templates
    tpl_bad_value = ("[Error] The asset {} has a bad \"{}\". It must be bigger "
                     "than 0. The current value is {}")

    tpl_uuid_in_use = "The asset {} uses an uuid taken by {}"

    # Set of fields and types which define an asset
    asset_fields = [{'fname': "name",         'ptype': str},
                    {'fname': "type",         'ptype': str},
                    {'fname': "uuid",         'ptype': int},
                    {'fname': "max_size",     'ptype': int},
                    {'fname': "client_perms", 'ptype': list}]

    # Variable to store the valid asset UUIDs to check that it is unique in the
    # asset definition file
    used_uuids = {}

    for asset in asset_defs:
        validate_asset_fields(asset, asset_fields)

        if asset['uuid'] <= 0:
            raise Exception(tpl_bad_value.format(asset['name'], "uuid",
                                                 asset['uuid']))

        if asset['uuid'] in used_uuids:
            raise Exception(tpl_uuid_in_use.format(asset['name'],
                                                   used_uuids[asset['uuid']]))
        if asset['max_size'] <= 0:
            raise Exception(tpl_bad_value.format(asset['name'], "max_size",
                                                 asset['max_size']))

        validate_asset_perms_fields(asset['client_perms'], asset['name'])

        # Store valid asset UUID to check that it is unique in the asset
        # definition file
        used_uuids[asset['uuid']] = asset['name']

def generate_asset_header(asset_defs, output_filename):
    """Generates C header file of asset definitions.

    asset_defs -- Asset definitions
    output_filename -- Filename of the output file <output_filename>.h

    Returns client_id_list
    """

    # Generate defines for asset UUIDs, asset maximum sizes
    max_size = 0
    client_id_list = []
    asset_ids_str = ""
    asset_sizes_str = ""
    id_tpl_str = "#define SST_ASSET_ID_{} {}\n"
    max_tpl_str = "#define SST_ASSET_MAX_SIZE_{} {}\n"
    for asset in asset_defs:
        asset_ids_str += id_tpl_str.format(asset['name'], asset['uuid'])
        asset_sizes_str += max_tpl_str.format(asset['name'], asset['max_size'])
        if asset['max_size'] > max_size:
            max_size = asset['max_size']

        for client in asset['client_perms']:
            if not client['client_id'] in client_id_list:
                client_id_list.append(client['client_id'])

    # Remove last '\n'
    asset_ids_str = asset_ids_str[:-1]
    asset_sizes_str = asset_sizes_str[:-1]

    # Sort list in descending order
    client_id_list.sort(reverse=True)

    # Generate defines for client IDs
    client_id_tpl_str = "#define SST_CLIENT_ID_{} {}\n"
    client_ids_str = ""
    count = 0
    for client_id in client_id_list:
        client_ids_str += client_id_tpl_str.format(count, client_id)
        count += 1

    # Remove last '\n'
    client_ids_str = client_ids_str[:-1]

    # Create dictionary with the keys and content to replace in the template
    tpl_dict = {'asset_ids': asset_ids_str,
                'asset_sizes': asset_sizes_str,
                'client_ids': client_ids_str,
                'number_of_assets': str(len(asset_defs)),
                'max_asset_size': str(max_size) }

    write_asset_file("header", tpl_dict, output_filename)

    return client_id_list

def generate_asset_source(asset_defs, client_id_list, output_filename):
    """Generates C source file of asset definitions.

    asset_defs -- Asset definitions
    output_filename -- Filename of the output file <output_filename>.c
    """

    # Template to define an asset
    tpl_asset_def = ("{\n"
                     "    .type = %s,\n"
                     "    .asset_uuid = SST_ASSET_ID_%s,\n"
                     "    .max_size = SST_ASSET_MAX_SIZE_%s,\n"
                     "    .perms_count = %d,\n"
                     "    .perms_modes_start_idx = %d,\n"
                     "},")

    start_idx = 0
    asset_defs_str = ""
    for asset in asset_defs:
        asset_defs_str += tpl_asset_def % (asset['type'],
                                           asset['name'],
                                           asset['name'],
                                           len(asset['client_perms']),
                                           start_idx)
        start_idx = start_idx + len(asset['client_perms'])

    # Remove last comma
    asset_defs_str = asset_defs_str[:-1]

    # Template to define the asset permissions
    tpl_asset_perms = ("{\n"
                       "    .client_id = SST_CLIENT_ID_%d,\n"
                       "    .perm = %s,\n"
                       "},")

    asset_perms_str = ""
    for asset in asset_defs:
        for client in asset['client_perms']:
            client_perms = ""
            count = 1
            for flag in client['perms']:
                client_perms = client_perms + "SST_PERM_{}".format(flag)
                if count < len(client['perms']):
                    client_perms = client_perms + " | "
                    count += 1

            asset_perms_str += tpl_asset_perms % (
                                      client_id_list.index(client['client_id']),
                                      client_perms)
    # Remove last comma
    asset_perms_str = asset_perms_str[:-1]

    # Create dictionary with the keys and content to replace in the template
    tpl_dict = {'asset_defs': asset_defs_str,
                'asset_perms': asset_perms_str }

    write_asset_file("source", tpl_dict, output_filename)

def generate_c_assets_files(yaml_file, output_filename):
    """Generates asset files based on the asset definitions in YAML format.

    yaml_file -- YAML file with the asset definitions
    output_filename -- Filename of the output files <output_filename>.c/h
    """

    # Open YAML file
    pFile = open(yaml_file, 'r')

    # Parse YAML file
    asset_defs = yaml.load(pFile.read())

    # Validate asset definitions from YAML file
    validate_asset_defs(asset_defs)

    # Generate header and source files
    client_id_list = generate_asset_header(asset_defs, output_filename)
    generate_asset_source(asset_defs, client_id_list, output_filename)

    print ("SST policy DB files (sst_asset_defs.c/h) have been generated "
           "correctly")

def get_cmd_arguments():
    """Gets command line arguments
    """

    # Define help descriptions
    app_desc = ("Generates the SST policy DB based on the SST asset definitions"
                " described in a YAML file.")
    yaml_file_desc = ("YAML filename which contains the asset definitions. "
                      "Default: {}".format(os.path.basename(default_yaml_file)))
    path_desc  = ("Directory to store the generated files. "
                  "Default: {}".format(default_output_path))
    ofile_desc = ("Output filename without path. "
                  "Default: {}".format(default_output_filename))

    parser = argparse.ArgumentParser(description=app_desc)

    # Set optional parameters
    parser.add_argument('-v', '--version', action='version',
                        version= "%(prog)s " + py_app_version)
    parser.add_argument('-i', '--ifile', default = default_yaml_file,
                        help = yaml_file_desc)
    parser.add_argument('-d', '--dir', default = default_output_path,
                        help = path_desc)
    parser.add_argument('-o', '--ofile', default = default_output_filename,
                        help = ofile_desc)

    # Parse arguments
    args = parser.parse_args()

    # Validate input arguments
    if not ".yaml" in args.ifile or not os.path.exists(args.ifile):
        print ("[Error] Missing YAML file extension: '%s'" % args.ifile)
        exit(1)

    if not os.path.exists(args.ifile):
        print ("[Error] No such file: '%s'" % args.ifile)
        exit(1)

    if not os.path.isdir(args.dir):
        print ("[Error] No such directory: '%s'" % args.dir)
        exit(1)

    if "/" in args.ofile:
        print ("[Error] Bad filename format: '%s'. The filename must not "
               "contain the output path and either extension" % args.ofile)
        exit(1)

    return args

if __name__ == '__main__':
    # Get absolute paths
    root_path = py_app_path.split("tools")[0]

    # Add absolute paths to the default values
    default_yaml_file = py_app_path + default_yaml_file
    default_output_path = root_path + default_output_path

    # Get arguments
    args = get_cmd_arguments()

    # Create output filename
    output_filename = os.path.normcase(args.dir + "/" + args.ofile)

    # Generate files
    try:
        generate_c_assets_files(args.ifile, output_filename)
    except yaml.YAMLError as exc:
      print ("[Error] YAML format issue " + str(exc))
    except BaseException as exc:
        print (exc)
        exit(1)
