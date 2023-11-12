//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_ERRORS_H
#define ATOMIC_ERRORS_H

#include "SharedIncludes/Colours.h"

#define SUCCESS 0
#define ARGERR 1
#define ENOMEM 12

//Errros
#define ERROR_BASE C_RED"ERROR"C_RST": "

#define ATOM_CT__CLI_ERR_ARG_NONE       ERROR_BASE"The ATOMIC compiler requires at least one argument (file to compile)\n"
#define ATOM_CT__CLI_ERR_ARG_NULL       ERROR_BASE"Error with argv, null pointer in first argument (bar program name)\n"

#define ATOM_CT__CLI_ERR_OPT_ARG        ERROR_BASE"Option \"%s\" require at least 1 argument\n"

//Warnings
#define WARNING_BASE C_YLW"WARNING"C_RST": "

#define ATOM_CT__CLI_WRN_OPT_FLG_INVLD  WARNING_BASE"Invalid flag given \"%s\"\n"

#define ATOM_CT__CLI_WRN_OPT_ARG_COUNT  WARNING_BASE"Option \"-%s\" only acts on 1 argument, ignoring others\n"
#define ATOM_CT__CLI_WRN_OPT_ARG_INVLD  WARNING_BASE"Option \"-%s\" given invalid argument \"%s\". Ignoring\n"

//Strings
#define ATOM_CT__CLI_OPT_OUT_TOK "tok"

#endif //ATOMIC_ERRORS_H
