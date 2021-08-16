#-------------------------------------------------------------------------------
# Copyright (c) 2018-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import os
import io
import sys
import argparse
from jinja2 import Environment, BaseLoader, select_autoescape, TemplateNotFound

try:
    import yaml
except ImportError as e:
    print (str(e) + " To install it, type:")
    print ("pip install PyYAML")
    exit(1)

donotedit_warning = \
                    "/*********** " + \
                    "WARNING: This is an auto-generated file. Do not edit!" + \
                    " ***********/"

OUT_DIR = None # The root directory that files are generated to

class TemplateLoader(BaseLoader):
    """
    Template loader class.

    An instance of this class is passed to the template engine. It is
    responsible for reading the template file
    """
    def __init__(self):
        pass

    def get_source(self, environment, template):
        """
        This function reads the template files.
        For detailed documentation see:
        http://jinja.pocoo.org/docs/2.10/api/#jinja2.BaseLoader.get_source

        Please note that this function always return 'false' as 'uptodate'
        value, so the output file will always be generated.
        """
        if not os.path.isfile(template):
            raise TemplateNotFound(template)
        with open(template) as f:
            source = f.read()
        return source, template, False

def manifest_validation(partition_manifest):
    """
    This function validates FF-M compliance for partition manifest, and sets
    default values for optional attributes.
    More validation items will be added.
    """
    # Service FF-M manifest validation
    if 'services' not in partition_manifest.keys():
        return partition_manifest

    for service in partition_manifest['services']:
        if 'version' not in service.keys():
            service['version'] = 1
        if 'version_policy' not in service.keys():
            service['version_policy'] = "STRICT"

    return partition_manifest

def process_partition_manifests(manifest_list_files):
    """
    Parse the input manifest, generate the data base for genereated files
    and generate manifest header files.

    Parameters
    ----------
    manifest_list_files:
        The manifest lists to parse.

    Returns
    -------
    The partition data base.
    """

    partition_list = []
    manifest_list = []

    for f in manifest_list_files:
        with open(f) as manifest_list_yaml_file:
            manifest_dic = yaml.safe_load(manifest_list_yaml_file)
            manifest_list.extend(manifest_dic["manifest_list"])
            manifest_list_yaml_file.close()

    pid_list = []
    no_pid_manifest_idx = []
    for i, manifest_item in enumerate(manifest_list):
        # Check if partition ID is manually set
        if 'pid' not in manifest_item.keys():
            no_pid_manifest_idx.append(i)
            continue
        # Check if partition ID is duplicated
        if manifest_item['pid'] in pid_list:
            raise Exception("PID No. {pid} has already been used!".format(pid=manifest_item['pid']))
        pid_list.append(manifest_item['pid'])
    # Automatically generate PIDs for partitions without PID
    pid = 256
    for idx in no_pid_manifest_idx:
        while pid in pid_list:
            pid += 1
        manifest_list[idx]['pid'] = pid
        pid_list.append(pid)

    for manifest_item in manifest_list:
        # Replace environment variables in the manifest path
        manifest_path = os.path.expandvars(manifest_item['manifest'])
        file = open(manifest_path)
        manifest = manifest_validation(yaml.safe_load(file))
        file.close()

        manifest_dir, manifest_name = os.path.split(manifest_path)
        manifest_out_basename = manifest_name.replace('.yaml', '').replace('.json', '')
        manifest_head_file = os.path.join(manifest_dir, "psa_manifest", manifest_out_basename + '.h').replace('\\', '/')
        intermedia_file = os.path.join(manifest_dir, "auto_generated", 'intermedia_' + manifest_out_basename + '.c').replace('\\', '/')
        load_info_file = os.path.join(manifest_dir, "auto_generated", 'load_info_' + manifest_out_basename + '.c').replace('\\', '/')

        if OUT_DIR is not None:
            """
            Remove the `source_path` portion of the filepaths, so that it can be
            interpreted as a relative path from the OUT_DIR.
            """
            if 'source_path' in manifest_item:
                # Replace environment variables in the source path
                source_path = os.path.expandvars(manifest_item['source_path'])
                manifest_head_file = os.path.relpath(manifest_head_file, start = source_path)
                intermedia_file = os.path.relpath(intermedia_file, start = source_path)
                load_info_file = os.path.relpath(load_info_file, start = source_path)

            manifest_head_file = os.path.join(OUT_DIR, manifest_head_file).replace('\\', '/')
            intermedia_file = os.path.join(OUT_DIR, intermedia_file).replace('\\', '/')
            load_info_file = os.path.join(OUT_DIR, load_info_file).replace('\\', '/')

        partition_list.append({"manifest": manifest, "attr": manifest_item,
                               "manifest_out_basename": manifest_out_basename,
                               "header_file": manifest_head_file,
                               "intermedia_file": intermedia_file,
                               "loadinfo_file": load_info_file})

    return partition_list

def gen_per_partition_files(context):
    """
    Generate per-partition files

    Parameters
    ----------
    context:
        context contains partition infos
    """

    subutilities = {}
    subutilities['donotedit_warning'] = donotedit_warning

    subcontext = {}
    subcontext['utilities'] = subutilities

    manifesttemplate = ENV.get_template('secure_fw/partitions/manifestfilename.template')
    memorytemplate = ENV.get_template('secure_fw/partitions/partition_intermedia.template')
    infotemplate = ENV.get_template('secure_fw/partitions/partition_load_info.template')

    print ("Start to generate partition files:")

    for one_partition in context['partitions']:
        subcontext['manifest'] = one_partition['manifest']
        subcontext['attr'] = one_partition['attr']
        subcontext['manifest_out_basename'] = one_partition['manifest_out_basename']

        print ("Generating Header: " + one_partition['header_file'])
        outfile_path = os.path.dirname(one_partition['header_file'])
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        headerfile = io.open(one_partition['header_file'], "w", newline=None)
        headerfile.write(manifesttemplate.render(subcontext))
        headerfile.close()

        print ("Generating Intermedia: " + one_partition['intermedia_file'])
        intermediafile_path = os.path.dirname(one_partition['intermedia_file'])
        if not os.path.exists(intermediafile_path):
            os.makedirs(intermediafile_path)
        intermediafile = io.open(one_partition['intermedia_file'], "w", newline=None)
        intermediafile.write(memorytemplate.render(subcontext))
        intermediafile.close()

        print ("Generating Loadinfo: " + one_partition['loadinfo_file'])
        infofile_path = os.path.dirname(one_partition['loadinfo_file'])
        if not os.path.exists(infofile_path):
            os.makedirs(infofile_path)
        infooutfile = io.open(one_partition['loadinfo_file'], "w", newline=None)
        infooutfile.write(infotemplate.render(subcontext))
        infooutfile.close()

    print ("Per-partition files done:")

def gen_summary_files(context, gen_file_lists):
    """
    Generate files according to the gen_file_list

    Parameters
    ----------
    gen_file_lists:
        The lists of files to generate
    """
    file_list = []

    for f in gen_file_lists:
        with open(f) as file_list_yaml_file:
            file_list_yaml = yaml.safe_load(file_list_yaml_file)
            file_list.extend(file_list_yaml["file_list"])

    print("Start to generate file from the generated list:")
    for file in file_list:
        # Replace environment variables in the output filepath
        manifest_out_file = os.path.expandvars(file["output"])
        # Replace environment variables in the template filepath
        templatefile_name = os.path.expandvars(file["template"])

        if OUT_DIR is not None:
            manifest_out_file = os.path.join(OUT_DIR, manifest_out_file)

        print ("Generating " + manifest_out_file)

        outfile_path = os.path.dirname(manifest_out_file)
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        template = ENV.get_template(templatefile_name)

        outfile = io.open(manifest_out_file, "w", newline=None)
        outfile.write(template.render(context))
        outfile.close()

    print ("Generation of files done")

def process_stateless_services(partitions, stateless_index_max_num):
    """
    This function collects all stateless services together, and allocates
    stateless handles for them.
    Valid stateless handle in service will be converted to an index. If the
    stateless handle is set as "auto", or not set, framework will allocate a
    valid index for the service.
    Framework puts each service into a reordered stateless service list at
    position of "index". Other unused positions are left None.
    """
    collected_stateless_services = []

    # Collect all stateless services first.
    for partition in partitions:
        # Skip the FF-M 1.0 partitions
        if partition['manifest']['psa_framework_version'] < 1.1:
            continue
        for service in partition['manifest']['services']:
            if 'connection_based' not in service:
                raise Exception("'connection_based' is mandatory in FF-M 1.1 service!")
            if service['connection_based'] is False:
                collected_stateless_services.append(service)

    if len(collected_stateless_services) == 0:
        return []

    if len(collected_stateless_services) > stateless_index_max_num:
        raise Exception("Stateless service numbers range exceed {number}.".format(number=stateless_index_max_num))

    """
    Allocate an empty stateless service list to store services.
    Use "handle - 1" as the index for service, since handle value starts from
    1 and list index starts from 0.
    """
    reordered_stateless_services = [None] * stateless_index_max_num
    auto_alloc_services = []

    for service in collected_stateless_services:
        # If not set, it is "auto" by default
        if 'stateless_handle' not in service:
            auto_alloc_services.append(service)
            continue

        service_handle = service['stateless_handle']

        # Fill in service list with specified stateless handle, otherwise skip
        if isinstance(service_handle, int):
            if service_handle < 1 or service_handle > stateless_index_max_num:
                raise Exception("Invalid stateless_handle setting: {handle}.".format(handle=service['stateless_handle']))
            # Convert handle index to reordered service list index
            service_handle = service_handle - 1

            if reordered_stateless_services[service_handle] is not None:
                raise Exception("Duplicated stateless_handle setting: {handle}.".format(handle=service['stateless_handle']))
            reordered_stateless_services[service_handle] = service
        elif service_handle == 'auto':
            auto_alloc_services.append(service)
        else:
            raise Exception("Invalid stateless_handle setting: {handle}.".format(handle=service['stateless_handle']))

    # Auto-allocate stateless handle and encode the stateless handle
    for i in range(0, stateless_index_max_num):
        service = reordered_stateless_services[i]

        if service == None and len(auto_alloc_services) > 0:
            service = auto_alloc_services.pop(0)

        """
        Encode stateless flag and version into stateless handle
        bit 30: stateless handle indicator
        bit 15-8: stateless service version
        bit 7-0: stateless handle index
        """
        stateless_handle_value = 0
        if service != None:
            stateless_index = (i & 0xFF)
            stateless_handle_value |= stateless_index
            stateless_flag = 1 << 30
            stateless_handle_value |= stateless_flag
            stateless_version = (service['version'] & 0xFF) << 8
            stateless_handle_value |= stateless_version
            service['stateless_handle_value'] = '0x{0:08x}'.format(stateless_handle_value)
            service['stateless_handle_index'] = stateless_index

        reordered_stateless_services[i] = service

    return reordered_stateless_services

def parse_args():
    parser = argparse.ArgumentParser(description='Parse secure partition manifest list and generate files listed by the file list',
                                     epilog='Note that environment variables in template files will be replaced with their values')

    parser.add_argument('-o', '--outdir'
                        , dest='outdir'
                        , required=False
                        , default=None
                        , metavar='out_dir'
                        , help='The root directory for generated files, the default is TF-M root folder.')

    parser.add_argument('-m', '--manifest'
                        , nargs='+'
                        , dest='manifest_args'
                        , required=True
                        , metavar='manifest'
                        , help='A set of secure partition manifest lists to parse')

    parser.add_argument('-f', '--file-list'
                        , nargs='+'
                        , dest='gen_file_args'
                        , required=True
                        , metavar='file-list'
                        , help='These files descripe the file list to generate')

    args = parser.parse_args()
    manifest_args = args.manifest_args
    gen_file_args = args.gen_file_args

    return args

ENV = Environment(
        loader = TemplateLoader(),
        autoescape = select_autoescape(['html', 'xml']),
        lstrip_blocks = True,
        trim_blocks = True,
        keep_trailing_newline = True
    )

def main():
    """
    The entry point of the script.

    Generates the output files based on the templates and the manifests.
    """

    global OUT_DIR

    args = parse_args()

    manifest_args = args.manifest_args
    gen_file_args = args.gen_file_args
    OUT_DIR = args.outdir

    manifest_list = [os.path.abspath(x) for x in args.manifest_args]
    gen_file_list = [os.path.abspath(x) for x in args.gen_file_args]

    """
    Relative path to TF-M root folder is supported in the manifests
    and default value of manifest list and generated file list are relative to TF-M root folder as well,
    so first change directory to TF-M root folder.
    By doing this, the script can be executed anywhere
    The script is located in <TF-M root folder>/tools, so sys.path[0]<location of the script>/.. is TF-M root folder.
    """
    os.chdir(os.path.join(sys.path[0], ".."))

    partition_list = process_partition_manifests(manifest_list)

    utilities = {}
    utilities['donotedit_warning'] = donotedit_warning

    context = {}
    context['partitions'] = partition_list
    context['utilities'] = utilities
    context['stateless_services'] = process_stateless_services(partition_list, 32)

    gen_per_partition_files(context)
    gen_summary_files(context, gen_file_list)

if __name__ == "__main__":
    main()
