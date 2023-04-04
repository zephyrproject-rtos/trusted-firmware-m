#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
import yaml
import logging, logging.handlers
import os
import argparse

from yaml.loader import SafeLoader

# logging setup
logging.basicConfig(level=logging.ERROR, format='%(name)s - %(levelname)s - %(message)s')

parser = argparse.ArgumentParser()
parser.add_argument("--input_file", help="DMA programs high level yaml file", required=True)
parser.add_argument("--output_file", help="Output bin file", required=True)
args = parser.parse_args()

class Loader(yaml.SafeLoader):
    def __init__(self, stream):
        self._root = os.path.split(stream.name)[0]
        super(Loader, self).__init__(stream)

    # load nested yaml files (if any)
    def include(self, node):
        filename = os.path.join(self._root, self.construct_scalar(node))
        with open(filename, 'r') as f:
            return yaml.load(f, Loader)

Loader.add_constructor('!include', Loader.include)

# calculate header word from bit fields
def calculate_header_word(header):
    final_val=0
    for i in range(len(header)):
        for key in header[i]:
            val=header[i][key]
            final_val = final_val + (val << i)
    return final_val

def pad_hex(s):
    return '0x' + s[2:].zfill(8)

# load top level input config file
with open(args.input_file, 'r') as f:
   data = yaml.load(f, Loader)

program_count = len(data["program"])
logging.info('Number of DMA programs : %d ', program_count)
hex_codes=[]

for prog_idx in range(program_count):
    command_count = len(data["program"][prog_idx]["commands"])
    logging.info("Program " +str(prog_idx) + " has " +str(command_count) + " comamnds")
    for cmd_idx in range(command_count):
        command = data["program"][prog_idx]["commands"][cmd_idx]
        for key in command:
            if key == "Header":
                header = data["program"][prog_idx]["commands"][cmd_idx]["Header"]
                header_word = calculate_header_word(header)
                hex_codes.append(str(hex(header_word)))
                logging.debug("Calculated header word is %s",str(hex(header_word)))
            else:
                hex_codes.append(str(pad_hex(hex(command[key]))))
                logging.debug(str(key) +': '+ str(pad_hex(hex(command[key]))))

out_file = open(args.output_file, mode="wb")

for item in hex_codes:
    data = bytes.fromhex(item.replace('0x','').rstrip())
    # by default; binary file is little in big endian format; update to little endian for arm */
    swap_data = bytearray(data)
    swap_data.reverse()
    out_file.write(swap_data)

out_file.close()