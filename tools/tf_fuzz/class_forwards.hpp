/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* The .hpp files in this project being so self-referential, this file forward-
   declares all classes before any are attempted to be used. */

#ifndef CLASS_FORWARDS_HPP
#define CLASS_FORWARDS_HPP

using namespace std;

// find_or_create_asset.hpp:
enum class asset_search;
enum class psa_asset_usage;
enum class psa_asset_search;
enum class psa_asset_type;

// data_blocks.hpp:
class expect_info;
class set_data_info;
class asset_name_id_info;

// variables.hpp:
class variable_info;

// template_line.hpp:
class template_line;
class sst_template_line;
class key_template_line;
class policy_template_line;

// sst_template_line.hpp:
class set_sst_template_line;
class remove_sst_template_line;
class read_sst_template_line;

// crypto_template_line.hpp:
class set_key_template_line;
class remove_key_template_line;
class read_key_template_line;
class set_policy_template_line;
class read_policy_template_line;

// security.hpp:
class security;
class security_hash;

// psa_call.hpp:
class psa_call;
class sst_call;
class crypto_call;

// sst_call.hpp:
class sst_set_call;
class sst_get_call;
class sst_remove_call;

// crypto_call.hpp:
class policy_call;
class key_call;
class init_policy_call;
class reset_policy_call;
class set_policy_usage_call;
class add_policy_usage_call;
class set_policy_lifetime_call;
class set_policy_algorithm_call;
class set_policy_size_call;
class set_policy_type_call;
class get_policy_lifetime_call;
class get_policy_size_call;
class get_policy_type_call;
class get_policy_algorithm_call;
class get_policy_usage_call;
class print_policy_usage_call;
class get_key_policy_call;
class generate_key_call;
class create_key_call;
class copy_key_call;
class read_key_data_call;
class remove_key_call;


// ************************    INSERT THE NEW STUFF HERE!!


class destroy_key_call;

// psa_asset.hpp:
class psa_asset;

// sst_asset.hpp:
class sst_asset;

// crypto_asset.hpp:
class crypto_asset;
class key_asset;
class policy_asset;
class key_asset;

// boilerplate.hpp"
//enum class boilerplate_texts;  not really a "class," and no need to forward-reference it anyway
class boilerplate;

// gibberish.hpp:
class gibberish;

// tf_fuzz.hpp:
class tf_fuzz_info;

#endif  // #ifndef CLASS_FORWARDS_HPP
