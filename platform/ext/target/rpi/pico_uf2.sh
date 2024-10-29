#!/bin/bash
tfm_tests_dir=$1
build_dir=$2
spe_bin="${tfm_tests_dir}/tests_reg/spe/${build_dir}/bin"
ns_bin="${tfm_tests_dir}/tests_reg/${build_dir}/bin"
python uf2conv.py "${spe_bin}/bl2.bin" --base 0x10000000 --convert --output "${spe_bin}/bl2.uf2" --family 0xe48bff59
python uf2conv.py "${ns_bin}/../tfm_s_ns_signed.bin" --base 0x10011000 --convert --output "${spe_bin}/tfm_s_ns_signed.uf2" --family 0xe48bff59
python uf2conv.py "${spe_bin}/provisioning_bundle.bin" --base 0x1019F000 --convert --output "${spe_bin}/provisioning_bundle.uf2" --family 0xe48bff59
