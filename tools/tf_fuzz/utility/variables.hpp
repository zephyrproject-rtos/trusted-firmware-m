/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string>
#include <vector>

/* This file defines information to track regarding variables in the generated test
   code. */

#ifndef VARIABLES_HPP
#define VARIABLES_HPP

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.  However these in particular are mostly axiomatic:  Not
   dependent upon other classes. */


using namespace std;


/**********************************************************************************
  Class variable_info tracks everything we know about a given variable in the
  generated C code.
**********************************************************************************/

class variable_info
{
public:
    // Data members:
        /* The existence of this variable tracker means that the data variable and
           the length variable have been declared, but there are other variants on
           this variable that may or may not have been declared already.  Thus
           the *_declared bool(s) below. */
        bool hash_declared;  // true if the hash of this variable has been declared
        bool value_known;  // true if the variable's value can be known in simulation
        string name;  // variable name
        unsigned char value[2048];  // the current value of the variable
        int length;  // of the variable's value
        psa_asset_type type;  // type of info contained in the variable

    // Methods:
        variable_info (void);  // (default constructor)
        variable_info (  // (constructor with known name and type)
            string var_name, psa_asset_type var_type
        );
        ~variable_info (void);  // (destructor)

protected:
    // Data members:
};


#endif // VARIABLES_HPP

