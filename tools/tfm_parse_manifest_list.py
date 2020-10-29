#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
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

def process_manifest(manifest_list_files):
    """
    Parse the input manifest, generate the data base for genereated files
    and generate manifest header files.

    Parameters
    ----------
    manifest_list_files:
        The manifest lists to parse.

    Returns
    -------
    The manifest header list and the data base.
    """

    db = []
    manifest_header_list = []
    manifest_list = []

    for f in manifest_list_files:
        with open(f) as manifest_list_yaml_file:
            manifest_dic = yaml.safe_load(manifest_list_yaml_file)
            manifest_list.extend(manifest_dic["manifest_list"])

    templatefile_name = 'secure_fw/partitions/manifestfilename.template'
    template = ENV.get_template(templatefile_name)

    print("Start to generate PSA manifests:")
    for manifest_item in manifest_list:
        # Replace environment variables in the manifest path
        manifest_path = os.path.expandvars(manifest_item['manifest'])
        file = open(manifest_path)
        manifest = yaml.safe_load(file)

        db.append({"manifest": manifest, "attr": manifest_item})

        utilities = {}
        utilities['donotedit_warning']=donotedit_warning

        context = {}
        context['manifest'] = manifest
        context['attr'] = manifest_item
        context['utilities'] = utilities

        manifest_dir, manifest_name = os.path.split(manifest_path)
        outfile_name = manifest_name.replace('yaml', 'h').replace('json', 'h')
        context['file_name'] = outfile_name.replace('.h', '')
        outfile_name = os.path.join(manifest_dir, "psa_manifest", outfile_name).replace('\\', '/')

        """
        Remove the `source_path` portion of the filepaths, so that it can be
        interpreted as a relative path from the OUT_DIR.
        """
        if 'source_path' in manifest_item:
            # Replace environment variables in the source path
            source_path = os.path.expandvars(manifest_item['source_path'])
            outfile_name = os.path.relpath(outfile_name, start = source_path)

        manifest_header_list.append(outfile_name)

        if OUT_DIR is not None:
            outfile_name = os.path.join(OUT_DIR, outfile_name)

        outfile_path = os.path.dirname(outfile_name)
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        print ("Generating " + outfile_name)

        outfile = io.open(outfile_name, "w", newline=None)
        outfile.write(template.render(context))
        outfile.close()

    return manifest_header_list, db

def gen_files(context, gen_file_lists):
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
        outfile_name = os.path.expandvars(file["output"])
        # Replace environment variables in the template filepath
        templatefile_name = os.path.expandvars(file["template"])

        if OUT_DIR is not None:
            outfile_name = os.path.join(OUT_DIR, outfile_name)

        print ("Generating " + outfile_name)

        outfile_path = os.path.dirname(outfile_name)
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        template = ENV.get_template(templatefile_name)

        outfile = io.open(outfile_name, "w", newline=None)
        outfile.write(template.render(context))
        outfile.close()

    print ("Generation of files done")

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

    # Arguments could be relative path.
    # Convert to absolute path as we are going to change diretory later
    if OUT_DIR is not None:
        OUT_DIR = os.path.abspath(OUT_DIR)

    """
    Relative path to TF-M root folder is supported in the manifests
    and default value of manifest list and generated file list are relative to TF-M root folder as well,
    so first change directory to TF-M root folder.
    By doing this, the script can be executed anywhere
    The script is located in <TF-M root folder>/tools, so sys.path[0]<location of the script>/.. is TF-M root folder.
    """
    os.chdir(os.path.join(sys.path[0], ".."))

    manifest_header_list, db = process_manifest(manifest_list)

    utilities = {}
    context = {}

    utilities['donotedit_warning']=donotedit_warning
    utilities['manifest_header_list']=manifest_header_list

    context['manifests'] = db
    context['utilities'] = utilities

    gen_files(context, gen_file_list)

if __name__ == "__main__":
    main()
