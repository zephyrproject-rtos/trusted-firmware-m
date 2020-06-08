/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

%{
#include <iostream>
#include <vector>
#include <set>

#include "class_forwards.hpp"
#include "data_blocks.hpp"
#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_call.hpp"
#include "security_call.hpp"
#include "secure_template_line.hpp"
#include "sst_template_line.hpp"
#include "crypto_template_line.hpp"

/* These items are defined in tf_fuzz_grammar.l.  Note, however that, because
   of "name mangling," defining them as extern "C" may or may not be ideal,
   depending upon which compiler -- gcc vs. g++, compiles the output from lex.
   So far, it seems best without the extern "C", including also compiling
   under Visual Studio. */
/* extern "C"
{ */
  extern int yylineno;
  int yywrap() {return 1;}
  extern char yytext[];
  extern int yyleng;
/* } */

int yylex (void);
void yyerror (tf_fuzz_info *, const char *);
    /* Sends the yyparse() argument to yyerror(), probably, to print incorrect
       text it parsed. */

/* A few consts just to make code more comprehensible: */
const bool yes_fill_in_template = true;
const bool dont_fill_in_template = false;
const bool yes_create_call = true;
const bool dont_create_call = false;

tf_fuzz_info *rsrc;

/* These are object pointers used to parse the template and create the test.  Ac-
   tually, probably only templateLin is used for now, but this is a good outline of
   of the template_line class hierarchy. */
template_line                   *templateLin = nullptr;
  sst_template_line             *sstTemplateLin = nullptr;
    set_sst_template_line       *setSstTemplateLin = nullptr;
    read_sst_template_line      *reaSstTemplateLin = nullptr;
    remove_sst_template_line    *remSstTemplateLin = nullptr;
  policy_template_line          *polTemplateLin = nullptr;
    set_policy_template_line    *setPolTemplateLin = nullptr;
    read_policy_template_line   *reaPolTemplateLin = nullptr;
  key_template_line             *keyTemplateLin = nullptr;
    set_key_template_line       *setKeyTemplateLin = nullptr;
    read_key_template_line      *reaKeyTemplateLin = nullptr;
    remove_key_template_line    *remKeyTemplateLin = nullptr;
  security_template_line        *secTemplateLin = nullptr;
    security_hash_template_line *secHasTemplateLin = nullptr;
/* Call and asset objects are presumably not immediately needed, because the objects
   of these types are within the resource object, *rsrc, but even if only just to
   show that class hierarchy: */
psa_call                        *psaCal = nullptr;
  sst_call                      *sstCal = nullptr;
    sst_set_call                *sstSetCal = nullptr;
    sst_get_call                *sstGetCal = nullptr;
    sst_remove_call             *sstRemCal = nullptr;
  crypto_call                   *cryCal = nullptr;
    policy_call                 *polCal = nullptr;
      init_policy_call          *iniPolCal = nullptr;
      reset_policy_call         *resPolCal = nullptr;
      add_policy_usage_call     *addPolUsaCal = nullptr;
      set_policy_lifetime_call  *setPolLifCal = nullptr;
      set_policy_type_call      *setPolTypCal = nullptr;
      set_policy_algorithm_call *setPolAlgCal = nullptr;
      set_policy_usage_call     *setPolUsaCal = nullptr;
      get_policy_lifetime_call  *getPolLifCal = nullptr;
      get_policy_type_call      *getPolTypCal = nullptr;
      get_policy_algorithm_call *getPolAlgCal = nullptr;
      get_policy_usage_call     *getPolUsaCal = nullptr;
      get_policy_size_call      *getPolSizCal = nullptr;
      get_key_policy_call       *getKeyPolCal = nullptr;
    key_call                    *keyCal = nullptr;
      generate_key_call         *genKeyCal = nullptr;
      create_key_call           *creKeyCal = nullptr;
      copy_key_call             *copKeyCal = nullptr;
      read_key_data_call        *reaKeyDatCal = nullptr;
      remove_key_call           *remKeyCal = nullptr;
psa_asset                       *psaAst = nullptr;
  sst_asset                     *sstAst = nullptr;
  crypto_asset                  *cryAst = nullptr;
    policy_asset                *polAst = nullptr;
    key_asset                   *keyAst = nullptr;

/* For generating random, but readable/memorable, data: */
gibberish gib;
char gib_buff[4096];  // spew gibberish into here
int rand_data_length = 0;

/* General-utility variables: */
bool purpose_defined = false;
psa_asset_usage random_asset = psa_asset_usage::all;
    /* to pick what type of asset at random */
bool random_name;  /* template didn't specify name, so it's generated randomly */
string literal_data;  /* literal data for an asset value */

/* Holders for state in read commands: */
expect_info expect;  /* everything about expected results and data */
set_data_info set_data;  /* everything about setting the value of PSA-asset data */
asset_name_id_info parsed_asset;  /* everything about identifying assets */
string target_barrier = "";  /* asset to set and search barrier when re-ordering PSA calls */
key_policy_info policy_info;  /* everything about key policies */
bool assign_data_var_specified = false;
string assign_data_var;
bool print_data = false;  /* true to just print asset data to the test log */
bool hash_data = false;  /* true to just print asset data to the test log */
bool literal_is_string = true;
    /* if true, literal value is character-string;  if false, is list of hex values */

/* The following are more tied to the template syntax than to the resulting PSA calls */
string literal;  /* temporary holder for all string literals */
string identifier;  /* temporary holder for strings representing identifiers */
string var_name;  /* a variable name */
string asset_name;  /* as parsed, not yet put into parsed_asset */
string aid;  /* string-typed holder for an asset ID in a list thereof */
int nid;  /* same idea as aid, but for asset ID# lists */
size_t strFind1, strFind2;  /* for searching through strings */

/* Because of the parsing order, psa_calls of the specific type have to be
   push_back()ed onto rsrc->calls before their expected results are known.  Therefore,
   must inject those results after parsing the expected results.  add_expect is a
   loop index to track where to add results. */
unsigned int add_expect = 0;

/* Temporaries: */
vector<psa_asset*>::iterator t_sst_asset;
vector<psa_asset*>::iterator t_key_asset;
vector<psa_asset*>::iterator t_policy_asset;
sst_call *t_sst_call = nullptr;
key_call *t_key_call = nullptr;
policy_call *t_policy_call = nullptr;
long number;  /* temporary holder for a number, e.g., sting form of UID */
int i, j, k;

/* Relating to template-statement blocks: */
vector<template_line*> template_block_vector;  /* (must be *pointers to* templates) */
vector<int> block_order;  /* "statisticalized" order of template lines in a block */
int nesting_level = 0;
    /* how many levels deep in { } nesting currently.  Initially only 0 or 1. */
bool shuffle_not_pick;
    /* true to shuffle statements in a block, rather than pick so-and-so
       number of them at random. */
int low_nmbr_lines = 1;  /* if picking so-and-so number of template lines from a ... */
int high_nmbr_lines = 1; /*    ... block at random, these are fewest and most lines. */
int exact_nmbr_lines = 1;

using namespace std;


void set_purp_str (
    char *raw_purpose,  /* the purpose C string from parser */
    tf_fuzz_info *rsrc  /* test resources containing the actual test-purpose string */
) {
    size_t l;  /* temporary of size_t type */
    string purp_str = raw_purpose;
    strFind1 = purp_str.find (" ");
    purp_str = purp_str.substr (strFind1, purp_str.length());
    purp_str.erase (0, 1);  // (extra space)
    strFind1 = purp_str.rfind (";");
    purp_str = purp_str.substr (0, strFind1);
    l = 0;
    do {  /* escape all " chars (if not already escaped) */
        l = purp_str.find ("\"", l);
        if (   l < purp_str.length()) {  /* did find a quote character */
            if (   l == 0  /* it's the first character in the string*/
                || purp_str[l-1] != '\\' /* or it's not already escaped */
               ) {
                purp_str.insert (l, "\\");  /* then escape the " char */
                l++;  /* point l to the " again */
            }
            l++;  /* point l past the " */
        }
    } while (l < purp_str.length());
    rsrc->test_purpose = purp_str;
}

/* randomize_template_lines() chooses a template-line order in cases where they are to
   be randomized -- shuffled or random picked. */
void randomize_template_lines (
    bool shuffle_not_pick,  /* true to perform a shuffle operation rather than pick */
    int &low_nmbr_lines, /* if picking so-and-so number of template lines from a ... */
    int &high_nmbr_lines, /*    ... block at random, these are fewest and most lines. */
    int &exact_nmbr_lines,
    vector<template_line*> &template_block_vector,
    vector<int> &block_order,
    tf_fuzz_info *rsrc  /* test resources containing the actual test-purpose string */
) {
    set<int> template_used;  /* used for shuffle */
    low_nmbr_lines = (low_nmbr_lines < 0)?  0 : low_nmbr_lines;
    high_nmbr_lines = (high_nmbr_lines < 0)?  0 : high_nmbr_lines;
    if (low_nmbr_lines > high_nmbr_lines) {
        int swap = low_nmbr_lines;
        low_nmbr_lines = high_nmbr_lines;
        high_nmbr_lines = swap;
    }
    template_used.clear();
    if (shuffle_not_pick) {
        /* Choose a random order in which to generate all of the
           template lines in the block: */
        while (template_used.size() < template_block_vector.size()) {
            i = rand() % template_block_vector.size();
            if (template_used.find (i) == template_used.end()) {
                /* This template not already shuffled in. */
                block_order.push_back (i);
                template_used.insert (i);
            }
        }
        /* Done shuffling;  empty out the set: */
    } else {
        if (high_nmbr_lines == low_nmbr_lines) {
            exact_nmbr_lines = low_nmbr_lines;
                /* just in case the template says "3 to 3 of"... */
        } else {
            exact_nmbr_lines =   low_nmbr_lines
                               + (rand() % (  high_nmbr_lines
                                            - low_nmbr_lines + 1  )  );
        }
        for (int j = 0;  j < exact_nmbr_lines;  ++j) {
            /* Repeatedly choose a random template line from the block: */
            i = rand() % template_block_vector.size();
            block_order.push_back (i);
        }
    }
    IVM(cout << "Order of lines in block:  " << flush;
        for (auto i : block_order) {
            cout << i << "  ";
        }
        cout << endl;
    )
}

/* interpret_template_line() fills in random data, locates PSA assets, (etc.) and
   conditionally creates PSA calls for a given template line.  Note that there needs
   to be a single place where all of this is done, so that statement blocks can be
   randomized and then dispatched from a single point. */
void interpret_template_line (
    template_line *templateLin,  /* the template line to process */
    tf_fuzz_info *rsrc,  /* program resources in general */
    set_data_info &set_data, psa_asset_usage random_asset,
    bool assign_data_var_specified, expect_info &expect, key_policy_info &policy_info,
    bool print_data, bool hash_data, string asset_name, string assign_data_var,
    asset_name_id_info &asset_info,  /* everything about the asset(s) involved */
    bool create_call_bool,  /* true to create the PSA call at this time */
    bool create_asset_bool,  /* true to create the PSA asset at this time */
    bool fill_in_template,  /* true to back-fill info into template */
    int instance
        /* if further differentiation to the names or IDs is needed, make instance >0 */
) {
    const bool yes_fill_in_template = true;  /* just to improve readability */
    vector<psa_asset*>::iterator t_psa_asset;

    if (fill_in_template) {
        /* Set basic parameters from the template line: */
        templateLin->set_data = set_data;
        templateLin->expect = expect;
        templateLin->policy_info = policy_info;
        templateLin->asset_info.id_n_not_name = asset_info.id_n_not_name;
        templateLin->asset_info.set_name (asset_name);
        /* Fill in state parsed from the template below: */
        templateLin->assign_data_var_specified = assign_data_var_specified;
        templateLin->assign_data_var.assign (assign_data_var);
        templateLin->print_data = print_data;
        templateLin->hash_data = hash_data;
        templateLin->random_asset = random_asset;
        if (   set_data.literal_data_not_file && !set_data.random_data
            && set_data.string_specified) {
            templateLin->set_data.set (literal_data);
        }
        /* Save names or IDs to the template-line tracker: */
        for (auto id_no : asset_info.asset_id_n_vector) {
             templateLin->asset_info.asset_id_n_vector.push_back (id_no);
        }
        asset_info.asset_id_n_vector.clear();
        for (auto as_name : asset_info.asset_name_vector) {
             templateLin->asset_info.asset_name_vector.push_back (as_name);
        }
        asset_info.asset_name_vector.clear();
    }

    /* Random asset choice (e.g., *active) case: */
    if (templateLin->random_asset != psa_asset_usage::all) {
        /* Just create the call tracker;  random name chosen in simulation stage: */
        templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                 yes_fill_in_template, create_call_bool,
                                 templateLin, rsrc   );
    } else if (asset_info.id_n_not_name) {
        /* Not random asset;  asset(s) by ID rather than name.  Go through all
           specified asset IDs: */
        uint64_t id_no;
        for (auto id_n :  templateLin->asset_info.asset_id_n_vector) {
            id_no = id_n + (uint64_t) instance * 10000UL;
            templateLin->asset_info.set_id_n(id_no);  /* just a holder */
            asset_name = templateLin->asset_info.make_id_n_based_name (id_no);
            templateLin->asset_info.set_calc_name (asset_name);
            templateLin->expect.data_var = var_name;
            if (!set_data.literal_data_not_file) {
                templateLin->set_data.set_file (set_data.file_path);
            }
            templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                     fill_in_template, create_call_bool,
                                     templateLin, rsrc   );
        }
    } else {
        /* Not random asset, asset(s) specified by name.  Go through all specified
           asset names: */
        for (auto as_name :  templateLin->asset_info.asset_name_vector) {
            /* Also copy into template line object's local vector: */
            if (instance > 0) {
                templateLin->asset_info.set_name (as_name + "_" + to_string (instance));
            } else {
                templateLin->asset_info.set_name (as_name);
            }
            /* Give each occurrence a different random ID: */
            templateLin->asset_info.set_id_n (100 + (rand() % 10000));
                /* TODO:  unlikely, but this *could* alias! */
            templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                     yes_fill_in_template, create_call_bool,
                                     templateLin, rsrc   );
        }
    }
}

%}

%start lines

%union {int valueN; int tokenN; char *str;}
%token <tokenN> PURPOSE RAW_TEXT
%token <tokenN> SET READ REMOVE SECURE DONE  /* root commands */
%token <tokenN> SST KEY POLICY NAME UID STAR ACTIVE DELETED EQUAL DATA DFNAME
%token <tokenN> FLAG NONE WRITE_ONCE NO_RP NO_CONF  /* SST creation flag keywords */
%token <tokenN> OFFSET  /* offset into an SST asset */
%token <tokenN> CHECK VAR HASH NEQ PRINT EXPECT PASS FAIL NOTHING ERROR  /* expected results */
%token <str> IDENTIFIER_TOK LITERAL_TOK HEX_LIST FILE_PATH_TOK  /* variables and content */
%token <valueN> NUMBER_TOK  /* variables and content */
%token <tokenN> SEMICOLON SHUFFLE TO OF OPEN_BRACE CLOSE_BRACE  /* block structure */
%token <tokenN> ATTR TYPE ALG  /* "set policy" line portions */
%token <tokenN> EXPORT COPY ENCRYPT DECRYPT SIGN VERIFY DERIVE  /* key-usage keywords */
%token <tokenN> NOEXPORT NOCOPY NOENCRYPT NODECRYPT NOSIGN NOVERIFY NODERIVE
%token <tokenN> PERSISTENT VOLATILE  /* key lifetime keywords */
%token <tokenN> FROM  /* for copying a key "from" another */
%token <tokenN> WITH  /* for specifying a key without explicitly defining a policy */

%define parse.error verbose
%locations
%parse-param {tf_fuzz_info *rsrc}

%%

  /* Top-level syntax: */
lines:
        %empty  /* nothing */
      | line lines {
            IVM(cout << "Lines:  Line number " << dec << yylineno << "." << endl;)
            /* Re-randomize objects we parse into: */
            expect = expect_info();
            set_data = set_data_info();
            parsed_asset = asset_name_id_info();
            policy_info = key_policy_info();
        }
      ;

line:
        PURPOSE {
            IVM(cout << "Purpose line:  " << flush;)
            set_purp_str (yytext, rsrc);
            IVM(cout << rsrc->test_purpose << endl;)
            /* TODO:  Is there much/any value in turning this back on?  The
                      constructor clear()s them out, and run-time errors observed
                      under Visual Studio...
               Just a precaution to make sure that these vectors start out empty.
               Should inherently be, but purpose is typically specified first:
            parsed_asset.asset_id_n_vector.clear();
            parsed_asset.asset_name_vector.clear(); */
            /* Re-randomize or re-initialize objects we parse into: */
            purpose_defined = true;
            expect = expect_info();
            set_data = set_data_info();
            parsed_asset = asset_name_id_info();
            policy_info = key_policy_info();
            target_barrier = "";
        }
      | block {
            /* TODO:  This code may not won't work with "secure hash neq ..." */
            IVM(cout << "Block of lines." << endl;)
            /* "Statisticalize" :-) the vector of template lines, then crank
               the selected lines in order here. */
            randomize_template_lines (shuffle_not_pick,
                low_nmbr_lines, high_nmbr_lines, exact_nmbr_lines,
                template_block_vector, block_order, rsrc
            );
            /* Vector block_order contains the sequence of template lines to be
               realized, in order.  Pop the indicated template line off the
               vector and generate code from it: */
            k = 0;  /* ID adder to at least help ensure uniqueness */
            for (int i : block_order) {
                templateLin = template_block_vector[i];
                /* Note that temLin will have its fields filled in already. */
                interpret_template_line (
                    templateLin, rsrc, set_data, random_asset,
                    assign_data_var_specified, expect, policy_info,
                    print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                    yes_create_call,  /* did not create call nor asset earlier */
                    yes_create_asset,
                    dont_fill_in_template,  /* but did fill it all in before */
                    0
                );
                k++;
                for (add_expect = 0;  add_expect < rsrc->calls.size();  ++add_expect) {
                    if (!(rsrc->calls[add_expect]->exp_data.expected_results_saved)) {
                        templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                        templateLin->expect.expected_results_saved = true;
                    }
                }
            }
            templateLin->asset_info.asset_id_n_vector.clear();
            templateLin->asset_info.asset_name_vector.clear();
            /* Done.  Empty out the "statisticalization" vector: */
            block_order.clear();
            /* Empty out the vector of template lines; no longer needed. */
            template_block_vector.clear();
            --nesting_level;
            IVM(cout << "Finished coding block of lines." << endl;)
        }
      | command SEMICOLON {
            IVM(cout << "Command with no expect:  \"" << flush;)
            if (!purpose_defined) {
                cerr << endl << endl
                     << "Error:  Please begin your test with the \"purpose\" "
                     << "directive.  \n        For example, "
                     << "\"purpose to exercise crypto and SST...\"" << endl;
                exit (1024);
            }
            if (nesting_level == 0) {  /* if laying down the code now... */
                for (add_expect = 0;  add_expect < rsrc->calls.size();  ++add_expect) {
                    if (!(rsrc->calls[add_expect]->exp_data.expected_results_saved)) {
                        templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                        templateLin->expect.expected_results_saved = true;
                    }
                }
                delete templateLin;  /* done with this template line */
            } else {
                /* The template line is now fully decoded, so stuff it onto
                   vector of lines to be "statisticalized": */
                template_block_vector.push_back (templateLin);
            }
            IVM(cout << yytext << "\"" << endl;)
        }
      | command expect SEMICOLON {
            /* (This is the same as for command SEMICOLON, other than the IVM.) */
            IVM(cout << "Command with expect:  \"" << flush;)
            if (!purpose_defined) {
                cerr << endl << endl
                     << "Error:  Please begin your test with the \"purpose\" "
                     << "directive.  \n        For example, "
                     << "\"purpose to exercise crypto and SST...\"" << endl;
                exit (1024);
            }
            if (nesting_level == 0) {
                for (add_expect = 0;  add_expect < rsrc->calls.size();  ++add_expect) {
                    if (!(rsrc->calls[add_expect]->exp_data.expected_results_saved)) {
                        templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                        templateLin->expect.expected_results_saved = true;
                    }
                }
                delete templateLin;
            } else {
                template_block_vector.push_back (templateLin);
            }
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

command:
        set_command {
            IVM(cout << "Set command:  \"" << yytext << "\"" << endl;)
        }
      | remove_command {
            IVM(cout << "Remove command:  \"" << yytext << "\"" << endl;)
        }
      | read_command {
            IVM(cout << "Read command:  \"" << yytext << "\"" << endl;)
        }
      | secure_command {
            IVM(cout << "Security command:  \"" << yytext << "\"" << endl;)
        }
      | done_command {
            IVM(cout << "Done command:  \"" << yytext << "\"" << endl;)
        }
      ;

expect:
        EXPECT PASS  {
            IVM(cout << "Expect pass clause:  \"" << flush;)
            templateLin->expect.set_pf_pass();
            IVM(cout << yytext << "\"" << endl;)
        }
      | EXPECT FAIL {
            IVM(cout << "Expect fail clause:  \"" << flush;)
            templateLin->expect.set_pf_fail();
            IVM(cout << yytext << "\"" << endl;)
        }
      | EXPECT NOTHING {
            IVM(cout << "Expect nothing clause:  \"" << flush;)
            templateLin->expect.set_pf_nothing();
            IVM(cout << yytext << "\"" << endl;)
        }
      | EXPECT IDENTIFIER {
            IVM(cout << "Expect error clause:  \"" << flush;)
            templateLin->expect.set_pf_error (identifier);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

  /* Root commands: */
set_command:
        SET SST sst_set_base_args sst_set_extended_args {
            IVM(cout << "Set SST command:  \"" << yytext << "\"" << endl;)
            templateLin = new set_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
        }
      | SET KEY key_set_args {
            IVM(cout << "Set key command:  \"" << yytext << "\"" << endl;)
            templateLin = new set_key_template_line (rsrc);
            target_barrier = policy_info.asset_2_name;  /* policy */
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
        }
      | SET POLICY policy_set_args {
            IVM(cout << "Set policy command:  \"" << yytext << "\"" << endl;;)
            templateLin = new set_policy_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
        }
      ;

read_command:
        READ SST sst_read_args {
            IVM(cout << "Read SST command:  \"" << yytext << "\"" << endl;;)
            templateLin = new read_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
        }
      | READ KEY key_read_args {
            IVM(cout << "Read key command:  \"" << yytext << "\"" << endl;;)
            templateLin = new read_key_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
        }
      | READ POLICY policy_read_args {
            IVM(cout << "Read policy command:  \"" << yytext << "\"" << endl;;)
            templateLin = new read_policy_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
        }
      ;

remove_command:
        REMOVE SST sst_remove_args {
            IVM(cout << "Remove SST command:  \"" << yytext << "\"" << endl;;)
            templateLin = new remove_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* don't create an asset being deleted */,
                yes_fill_in_template, 0
            );
        }
      | REMOVE KEY key_remove_args {
            IVM(cout << "Remove key command:  \"" << yytext << "\"" << endl;;)
            templateLin = new remove_key_template_line (rsrc);
            templateLin->asset_info.set_name (asset_name);  // set in key_asset_name, below
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, policy_info,
                print_data, hash_data, asset_name, assign_data_var, parsed_asset,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* don't create an asset being deleted */,
                yes_fill_in_template, 0
            );
        }
      ;

secure_command: SECURE HASH NEQ ASSET_IDENTIFIER_LIST {
  /* TODO:  This needs to allow not only SST assets, but mix and match with others
             (keys especially) as well. */
            IVM(cout << "Secure hash command:  \"" << yytext << "\"" << endl;)
            templateLin = new security_hash_template_line (rsrc);
            templateLin->asset_info.set_name (asset_name);
            templateLin->assign_data_var_specified = assign_data_var_specified;
            templateLin->assign_data_var.assign (assign_data_var);
            templateLin->expect = expect;
            templateLin->print_data = print_data;
            templateLin->hash_data = hash_data;
            templateLin->random_asset = random_asset;
            /* Hash checks are different from the rest in that there's a single
               "call" -- not a PSA call though -- for all of the assets cited in the
               template line.  In *other* cases, create a single call for *each*
               asset cited by the template line, but not in this case. */
            for (auto as_name : parsed_asset.asset_name_vector) {
                /* Also copy into template line object's local vector: */
                 templateLin->asset_info.asset_name_vector.push_back (as_name);
            }
            /* Don't need to locate the assets, so no searches required. */
            templateLin->expect.data_var = var_name;
            templateLin->setup_call (set_data, set_data.random_data, yes_fill_in_template,
                                     nesting_level == 0, templateLin, rsrc   );
            parsed_asset.asset_name_vector.clear();


        }
      ;

done_command: DONE {
            if (nesting_level != 0) {
                cerr << "\n\"done\" only available at outer-most { } nesting level."
                     << endl;
                exit (702);
            } else {
                YYACCEPT;
            }
        }
      ;

literal_or_random_data:
        DATA LITERAL {
            IVM(cout << "Create from literal data:  \"" << flush;)
            set_data.random_data = false;
            set_data.string_specified = true;
            set_data.literal_data_not_file = true;
            literal.erase(0,1);  // zap the ""s
            literal.erase(literal.length()-1,1);
            literal_data.assign (literal);
            IVM(cout << yytext << "\"" << endl;)
        }
      | DATA STAR {  /* TF-Fuzz supplies random data */
            IVM(cout << "Create from random data" << endl;)
            set_data.randomize();
            literal.assign (set_data.get());  /* just in case something uses literal */
            set_data.random_data = true;
            set_data.string_specified = false;
        }
      ;

  /* Root-command parameters: */
sst_set_base_args:
        sst_asset_name literal_or_random_data
      | sst_asset_name {
            IVM(cout << "SST-create from random data (no 'data *')" << endl;)
            set_data.randomize();
            literal.assign (set_data.get());  /* just in case something uses literal */
        }
      | sst_asset_name VAR IDENTIFIER {  /* set from variable */
            IVM(cout << "SST-set set from variable:  \"" << flush;)
            assign_data_var.assign (identifier);
            assign_data_var_specified = true;
            expect.data_specified = false;
            expect.data_var_specified = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | sst_asset_name DFNAME sst_asset_set_file_path {
            set_data.literal_data_not_file = set_data.random_data = false;
            IVM(cout << "SST-create from file:  " << yytext << "\"" << endl;)
            /* TODO:  Need to decide whether the concept of using files to set SST
                       asset values has meaning, and if so, write code to write code to
                       set data appropriately from the file. */
        }
      ;

sst_set_extended_args:
        %empty /* nothing */
      | FLAG sst_flags {
            IVM(cout << "SST creation flags" << endl;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_flags:
        %empty /* nothing */
      | sst_flag sst_flags {
            IVM(cout << "SST creation flag" << endl;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_flag:  none | write_once | no_rp | no_conf;

none : NONE {
            set_data.flags_string = "PSA_STORAGE_FLAG_NONE";
                /* TODO:  grab from boilerplate */
            IVM(cout << "SST no storage flag:  " << yytext << "\"" << endl;)
        }
      ;

write_once : WRITE_ONCE {
            set_data.flags_string = "PSA_STORAGE_FLAG_WRITE_ONCE";
                /* TODO:  grab from boilerplate */
            IVM(cout << "SST write-once flag:  " << yytext << "\"" << endl;)
        }
      ;

no_rp : NO_RP {
            set_data.flags_string = "PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION";
                /* TODO:  grab from boilerplate */
            IVM(cout << "SST no-replay-protection flag:  "
                     << yytext << "\"" << endl;)
        }
      ;

no_conf : NO_CONF {
            set_data.flags_string = "PSA_STORAGE_FLAG_NO_CONFIDENTIALITY";
                /* TODO:  grab from boilerplate */
            IVM(cout << "SST no-confidentiality flag:  " << yytext
                     << "\"" << endl;)
        }
      ;

sst_offset_spec:
        NUMBER_TOK {
            IVM(cout << "SST-data offset:  \"" << flush;)
            set_data.data_offset = atol(yytext);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;


sst_read_args:
        sst_asset_name read_args sst_read_extended_args {
            IVM(cout << "SST-read arguments:  " << yytext << "\"" << endl;)
        }
      ;

read_args:
        VAR IDENTIFIER {  /* dump to variable */
            IVM(cout << "Read dump to variable:  \"" << flush;)
            assign_data_var.assign (identifier);
            assign_data_var_specified = true;
            expect.data_specified = false;
            expect.data_var_specified = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | CHECK read_args_var_name {  /* check against variable */
            IVM(cout << "Read check against variable:  \""
                     << yytext << "\"" << endl;)
            set_data.set (literal);
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = true;
            expect.data_var = identifier;
        }
      | CHECK LITERAL {  /* check against literal */
            IVM(cout << "Read check against literal:  " << flush;)
            expect.data.assign (literal);
            expect.data.erase(0,1);    // zap the ""s
            expect.data.erase(expect.data.length()-1,1);
            assign_data_var_specified = false;  /* don't read variable */
            expect.data_specified = true;  /* check against literal data */
            expect.data_var_specified = false;  /* don't check against variable */
            IVM(cout << yytext << endl;)
        }
      | PRINT {  /* print out content in test log */
            IVM(cout << "Read log to test log:  \"" << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = false;
            print_data = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | HASH {  /* hash the data and save for later comparison */
            IVM(cout << "Read hash for future data-leak detection:  \"" << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = false;
            hash_data = true;
            rsrc->include_hashing_code = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | DFNAME sst_asset_dump_file_path {  /* dump to file */
            IVM(cout << "Read dump to file:  \""
                     << yytext << "\"" << endl;)
            set_data.literal_data_not_file = set_data.random_data = false;
        }
      ;

sst_read_extended_args:
        %empty /* nothing */
      | OFFSET sst_offset_spec {
            IVM(cout << "SST data offset" << endl;)
            set_data.data_offset = atol(yytext);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_remove_args:
        sst_asset_name | random_picked_asset {
            IVM(cout << "SST-remove arguments:  \""
                     << yytext << "\"" << endl;)
        }
      ;

asset_designator:
        NAME ASSET_IDENTIFIER_LIST {
            IVM(cout << "Asset identifier list:  \"" << flush;)
            random_name = false;
            asset_name.assign (identifier);  /* TODO:  Not sure this ultimately has any effect... */
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | NAME STAR {
            IVM(cout << "Asset random identifier:  \"" << flush;)
            random_name = true;
            rand_data_length = 4 + (rand() % 5);
            gib.word (false, gib_buff, gib_buff + rand_data_length - 1);
            aid.assign (gib_buff);
            parsed_asset.asset_name_vector.push_back (aid);
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;
single_existing_asset:
        IDENTIFIER {
            IVM(cout << "Single existing asset by name:  \"" << flush;)
            random_name = false;
            policy_info.asset_3_name.assign (identifier);
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | random_picked_asset
      ;

random_picked_asset:
        STAR ACTIVE {
            IVM(cout << "Asset random active:  \"" << flush;)
            random_asset = psa_asset_usage::active;
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | STAR DELETED {
            IVM(cout << "Asset random deleted:  \"" << flush;)
            random_asset = psa_asset_usage::deleted;
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_asset_name:
        asset_designator
      | UID ASSET_NUMBER_LIST {
            IVM(cout << "SST-asset UID list:  \"" << flush;)
            random_name = false;
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = true;
            parsed_asset.id_n_specified = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | UID STAR {
            IVM(cout << "SST-asset random UID:  \"" << flush;)
            parsed_asset.id_n_not_name = true;
            random_name = false;
            nid = 100 + (rand() % 10000);
            parsed_asset.asset_id_n_vector.push_back (nid);
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_asset_set_file_path:
        FILE_PATH {
            IVM(cout << "SST-asset-create file path:  \"" << flush;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

read_args_var_name:
        IDENTIFIER {
            IVM(cout << "Read-arguments variable name:  \"" << flush;)
            var_name = yytext;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_asset_dump_file_path:
        FILE_PATH {
            IVM(cout << "SST-asset dump-file path:  \"" << flush;)
            set_data.file_path = yytext;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

key_size:
        NUMBER_TOK {
            IVM(cout << "Key size:  \"" << flush;)
            policy_info.n_bits = atol(yytext);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

policy_usage_list:  ATTR policy_usage policy_usages;  /* at least one usage */

policy_usages:
        %empty  /* nothing */
      | policy_usage policy_usages {
            IVM(cout << "Key-policy usages at line number " << dec << yylineno
                     << "." << endl;)
        }
      ;

export : EXPORT {
            policy_info.exportable = true;
            IVM(cout << "Exportable key true:  " << yytext << "\"" << endl;)
        }
      ;

noexport : NOEXPORT {
            policy_info.exportable = false;
            IVM(cout << "Non-exportable key:  " << yytext << "\"" << endl;)
        }
      ;

copy : COPY {
            policy_info.copyable = true;
            IVM(cout << "Copyable key true:  " << yytext << "\"" << endl;)
        }
      ;

nocopy : NOCOPY {
            policy_info.copyable = false;
            IVM(cout << "Non-copyable key:  " << yytext << "\"" << endl;)
        }
      ;

encrypt : ENCRYPT {
            policy_info.can_encrypt = true;
            IVM(cout << "Encryption key true:  " << yytext << "\"" << endl;)
        }
      ;

noencrypt : NOENCRYPT {
            policy_info.can_encrypt = false;
            IVM(cout << "Non-encryption key:  " << yytext << "\"" << endl;)
        }
      ;

decrypt : DECRYPT {
            policy_info.can_decrypt = true;
            IVM(cout << "Decryption key true:  " << yytext << "\"" << endl;)
        }
      ;

nodecrypt : NODECRYPT {
            policy_info.can_decrypt = false;
            IVM(cout << "Non-decryption key:  " << yytext << "\"" << endl;)
        }
      ;

sign : SIGN {
            policy_info.can_sign = true;
            IVM(cout << "Signing key true:  " << yytext << "\"" << endl;)
        }
      ;

nosign : NOSIGN {
            policy_info.can_sign = false;
            IVM(cout << "Non-signing key:  " << yytext << "\"" << endl;)
        }
      ;

verify : VERIFY {
            policy_info.can_verify = true;
            IVM(cout << "Verify key true:  " << yytext << "\"" << endl;)
        }
      ;

noverify : NOVERIFY {
            policy_info.can_verify = false;
            IVM(cout << "Non-verify key:  " << yytext << "\"" << endl;)
        }
      ;

derive : DERIVE {
            policy_info.derivable = true;
            IVM(cout << "Derivable key true:  " << yytext << "\"" << endl;)
        }
      ;

noderive : NODERIVE {
            policy_info.derivable = false;
            IVM(cout << "Non-derivable key:  " << yytext << "\"" << endl;)
        }
      ;

persistent : PERSISTENT {
            policy_info.persistent = true;
            IVM(cout << "Persistent key:  " << yytext << "\"" << endl;)
        }
      ;

volatle : VOLATILE {
            policy_info.persistent = false;
            IVM(cout << "Volatile key:  " << yytext << "\"" << endl;)
        }
      ;

policy_usage:
        export | copy | encrypt | decrypt | sign | verify | derive
      | noexport | nocopy | noencrypt | nodecrypt | nosign | noverify
      | noderive | persistent | volatle | key_size {
            IVM(cout << "Policy usage:  " << yytext << "\"" << endl;)
        }
      ;

policy_type:
      TYPE IDENTIFIER {
            // Change type identifier, e.g., from "raw_data" to PSA_KEY_TYPE_RAW_DATA:
            identifier = formalize (identifier, "PSA_KEY_TYPE_");
            policy_info.key_type = identifier;
            IVM(cout << "Policy type:  \""
                     << policy_info.key_type << "\"" << endl;)
      }

policy_algorithm:
      ALG IDENTIFIER {
            // Change type identifier, e.g., from "sha_256" to PSA_ALG_SHA_256:
            identifier = formalize (identifier, "PSA_ALG_");
            policy_info.key_algorithm = identifier;
            IVM(cout << "Policy algorithm:  \""
                     << policy_info.key_algorithm << "\"" << endl;)
      }

policy_specs:
        %empty  /* nothing */
      | policy_spec policy_specs {
            IVM(cout << "Key-policy specs at line number " << dec << yylineno
                     << "." << endl;)
        }
      ;

policy_spec:  policy_usage_list | policy_type | policy_algorithm;

policy_asset_spec:
        %empty  /* nothing */
      | NAME ASSET_IDENTIFIER_LIST {
            IVM(cout << "policy-asset identifier list:  \"" << flush;)
            random_name = false;
            asset_name.assign (identifier);  /* TODO:  Not sure this ultimately has any effect... */
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | NAME STAR {
            IVM(cout << "policy-asset random identifier:  \"" << flush;)
            random_name = true;
            rand_data_length = 2 + (rand() % 10);
            gib.word (false, gib_buff, gib_buff + rand_data_length - 1);
            aid.assign (gib_buff);
            parsed_asset.asset_name_vector.push_back (aid);
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

policy_asset_name:
        NAME IDENTIFIER {
            IVM(cout << "policy-asset identifier list:  \"" << flush;)
            random_name = false;
            policy_info.get_policy_from_key = false;
            asset_name.assign (identifier);  /* TODO:  Not sure this ultimately has any effect... */
            parsed_asset.asset_name_vector.push_back (identifier);
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | STAR ACTIVE {
            IVM(cout << "policy-asset random active:  \"" << flush;)
            policy_info.get_policy_from_key = false;
            random_asset = psa_asset_usage::active;
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | STAR DELETED {
            IVM(cout << "policy-asset random deleted:  \"" << flush;)
            policy_info.get_policy_from_key = false;
            random_asset = psa_asset_usage::deleted;
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | KEY IDENTIFIER {
            IVM(cout << "policy-asset specified by key:  \"" << flush;)
            policy_info.get_policy_from_key = true;
            random_name = false;
            asset_name.assign (identifier);  /* ask this key what it's policy is */
            random_asset = psa_asset_usage::all;  /* don't randomly choose existing asset */
            parsed_asset.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

policy_set_args:
        policy_asset_spec policy_specs {
            IVM(cout << "Policy-create arguments:  \"" << yytext << "\"" << endl;)
        }
      ;

policy_read_args:
        policy_asset_name read_args {
            IVM(cout << "Policy-read arguments:  " << yytext << "\"" << endl;)
        }
      ;

key_set_sources:
        %empty  /* nothing */
      | key_set_source key_set_sources {
            IVM(cout << "Key-set sources at Line number "
                     << yytext << "\"" << endl;)
        }
      ;

key_set_source:
        literal_or_random_data {
            IVM(cout << "Key-set sources, literal or random data:  "
                     << yytext << "\"" << endl;)
        }
      | POLICY IDENTIFIER {
            IVM(cout << "Key-set sources, explicitly-specified policy name:  "
                     << flush;)
            policy_info.asset_2_name = identifier;  /* policy */
            /* Make note that key data (key material) was not specified: */
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

key_data_or_not:
        %empty  /* nothing */
      | literal_or_random_data {
            IVM(cout << "Key data, literal or random data:  "
                     << yytext << "\"" << endl;)
        }
      ;

key_set_args:
        asset_designator key_set_sources {
            IVM(cout << "Key-create from data, policy, or nothing (default):  \""
                     << yytext << "\"" << endl;)
            policy_info.copy_key = false;
            policy_info.implicit_policy = false;
        }
      | asset_designator FROM single_existing_asset POLICY IDENTIFIER {
            IVM(cout << "Key-copy from other key:  \"" << flush;)
            policy_info.asset_2_name = identifier;  /* policy */
            policy_info.copy_key = true;
            policy_info.implicit_policy = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | asset_designator key_data_or_not WITH policy_specs {
            IVM(cout << "Key-create directly specifying policy attributes (implicit policy):  \""
                     << yytext << "\"" << endl;)
            policy_info.copy_key = false;
            policy_info.implicit_policy = true;
            cerr << "\nError:  Defining keys with implicit policies is not yet implemented."
                 << endl;
            exit (772);
        }
      ;

key_remove_args:
        asset_designator {
            IVM(cout << "Key-remove arguments:  \""
                     << yytext << "\"" << endl;)
        }
      ;

key_read_args:
        asset_designator read_args {
            IVM(cout << "Key dump:  \"" << yytext << "\"" << endl;)
        }
      ;

/* Code structuring: */
block:
        SHUFFLE block_content {
            IVM(cout << "Shuffled block:  \"" << flush;)
            if (nesting_level > 1) {
                cerr << "\nError:  Sorry, currently only one level of { } "
                     << "nesting is allowed." << endl;
                exit (500);
            }
            shuffle_not_pick = true;
            low_nmbr_lines = high_nmbr_lines = 0;  /* not used, but... */
            IVM(cout << yytext << "\"" << endl;)
        }
      | exact_sel_count OF block_content {
            IVM(cout << "Fixed number of lines from block:  \"" << flush;)
            shuffle_not_pick = false;
            /* low_nmbr_lines and high_nmbr_lines are set below. */
            IVM(cout << yytext << "\"" << endl;)
        }
      | low_sel_count TO high_sel_count OF block_content {
            IVM(cout << "Range number of lines from block:  \"" << flush;)
            if (nesting_level > 1) {
                cerr << "\nError:  Sorry, currently only one level of { } "
                     << "nesting is allowed." << endl;
                exit (502);
            }
            shuffle_not_pick = false;
            /* low_nmbr_lines and high_nmbr_lines are set below. */
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

block_content:
        open_brace lines close_brace {
            IVM(cout << "Block content:  \"" << yytext << "\"" << endl;)
        }
      | line {
            IVM(cout << "Single-line would-be-block content:  \"" << flush;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

open_brace:
        OPEN_BRACE {
            IVM(cout << "Open brace:  \"" << flush;)
            template_block_vector.clear();  // clean slate of template lines
            nesting_level = 1;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

close_brace:
        CLOSE_BRACE {
            IVM(cout << "Close brace:  " << flush;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

  /* Low-level structures: */

  /* Please see comment before ASSET_IDENTIFIER_LIST, below. */
ASSET_NUMBER_LIST:
        ASSET_NUMBER ASSET_NUMBERS;  /* at least one number */

ASSET_NUMBERS:
        %empty  /* nothing */
      | ASSET_NUMBER ASSET_NUMBERS;

ASSET_NUMBER:
        NUMBER_TOK {
            IVM(cout << "ASSET_NUMBER:  \"" << flush;)
            nid = atol(yytext);
            parsed_asset.asset_id_n_vector.push_back (nid);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

  /* ASSET_IDENTIFIER are used specifically for lists of assets within a singletemplate
     line.  That, as opposed to list of identifers in general.  The difference is the
     need to queue ASSET_IDENTIFIERS up into parsed_asset.asset_name_vector, and have to
     do so here before they "vanish." */
ASSET_IDENTIFIER_LIST:
        ASSET_IDENTIFIER ASSET_IDENTIFIERS;  /* (at least one) */

ASSET_IDENTIFIERS:
        %empty  /* nothing */
      | ASSET_IDENTIFIER ASSET_IDENTIFIERS;

ASSET_IDENTIFIER:
        IDENTIFIER_TOK {
            IVM(cout << "ASSET_IDENTIFIER:  \"" << flush;)
            aid = identifier = yytext;
            parsed_asset.asset_name_vector.push_back (aid);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

IDENTIFIER:
        IDENTIFIER_TOK {
            IVM(cout << "IDENTIFIER:  \"" << flush;)
            identifier = yytext;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

FILE_PATH:
        FILE_PATH_TOK {
            IVM(cout << "FILE_PATH:  \"" << flush;)
            set_data.file_path = yytext;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

  /* These are related to randomized blocks of template lines: */

exact_sel_count:
        NUMBER {
            IVM(cout << "Exact number of random template lines:  \"" << flush;)
            low_nmbr_lines = high_nmbr_lines = exact_nmbr_lines = number;
            ++nesting_level;
            IVM(cout << number << "\"" << endl;)
        }
      ;

low_sel_count:
        NUMBER {
            IVM(cout << "Least number of random template lines:  \"" << flush;)
            low_nmbr_lines = number;
            IVM(cout << number << "\"" << endl;)
        }
      ;

high_sel_count:
        NUMBER {
            IVM(cout << "Most number of random template lines:  \"" << flush;)
            high_nmbr_lines = number;
            ++nesting_level;
            IVM(cout << number << "\"" << endl;)
        }
      ;


  /* These are general-case numbers, literals and such: */

NUMBER: NUMBER_TOK {
            IVM(cout << "NUMBER:  \"" << flush;)
            number = atol(yytext);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

LITERAL:
        LITERAL_TOK {
          IVM(cout << "LITERAL string:  " << flush;)
          literal = yytext;
          literal_is_string = true;
          IVM(cout << yytext << endl;)
        }
      | HEX_LIST {
          IVM(cout << "LITERAL hex-value list:  " << flush;)
          literal = yytext;
          literal_is_string = false;
          IVM(cout << yytext << endl;)
        }
      ;


%%

