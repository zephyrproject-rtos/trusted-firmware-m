#! /usr/bin/env python3
#
# -----------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import os
import click
from jinja2 import Environment, FileSystemLoader


@click.argument('outfile')
@click.option('--template_path', metavar='filename', required=True)
@click.option('--guk', metavar='key', required=False)
@click.option('--sam_config', metavar='config', required=False)
@click.command(help='''Creates a .c file with the given keys, using the\n
               provisioning_data_template.jinja2 template which is located in
               "template_path" and outputs it to "outfile"''')
def generate_provisioning_data_c(outfile, template_path, guk, sam_config):

    environment = Environment(loader=FileSystemLoader(template_path))
    template = environment.get_template("cm_provisioning_data_template.jinja2")

    if bool(guk) is False:
        guk = hex_to_c_array(os.urandom(32))

    if bool(sam_config) is False:
        sam_config = hex_to_c_array(bytes(23))

    key_arrays = {
        "guk": guk,
        "sam_config": sam_config,
    }

    with open(outfile, "w") as F:
        F.write(template.render(key_arrays))


def hex_to_c_array(hex_val):
    c_array = ""
    for count, b in enumerate(hex_val):
        if count % 8 == 0 and count != 0:
            c_array = c_array + '\n'
        c_array = c_array + "0x{:02x}, ".format(b)

    return c_array


if __name__ == '__main__':
    generate_provisioning_data_c()
