//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_ERRORS_H
#define ATOMIC_ERRORS_H

#include "SharedIncludes/Colours.h"

#define SUCCESS 0
#define ARGERR 1
#define ENOMEM 12


//https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html
//and if the variable arguments are omitted or empty, the ‘##’ operator causes the preprocessor to remove the comma before it.
#define VA_ARGS(...) , ##__VA_ARGS__

//Errros
#define PError(ErrorMsg, ...) printf(ERROR_BASE ErrorMsg VA_ARGS(__VA_ARGS__))

#define ATOM_CT__CLI_ERR_ARG_NONE       "The ATOMIC compiler requires at least one argument (file to compile)\n"
#define ATOM_CT__CLI_ERR_ARG_NULL       "Error with argv, null pointer in first argument (bar program name)\n"

#define ATOM_CT__CLI_ERR_OPT_ARG        "Option \"%s\" require at least 1 argument\n"

//Warnings
#define PWarn(WarningMsg, ...) printf(WARN_BASE WarningMsg VA_ARGS(__VA_ARGS__))

#define ATOM_CT__CLI_WRN_OPT_FLG_INVLD  "Invalid flag given \"%s\"\n"

#define ATOM_CT__CLI_WRN_OPT_ARG_COUNT  "Option \"-%s\" only acts on 1 argument, ignoring others\n"
#define ATOM_CT__CLI_WRN_OPT_ARG_INVLD  "Option \"-%s\" given invalid argument \"%s\". Ignoring\n"

//Strings
#define ATOM_CT__CLI_OPT_OUT_TOK "tok"

#endif //ATOMIC_ERRORS_H
