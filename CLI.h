//
// Created by jamescoward on 06/11/2023.
//

#ifndef ATOMIC_CLI_H
#define ATOMIC_CLI_H

#include "Commons.h"


#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Helper_String.h"
#include "Errors.h"
#include "Flags.h"
#include "Compiler.h"

typedef unsigned int uint;
typedef long long int llint;

#define ATOM_CT__CLI_DEFAULT_OUT                "out.a"
#define ATOM_CT__CLI_DEFAULT_FILE_BUFF_SIZE     4
#define ATOM_CT__CLI_DEFAULT_OPTION_BUFF_SIZE   10

#define ATOM_CT__CLI_SUCC                       SUCC_BASE"Compilation complete!\n"
#define ATOM_CT__CLI_ERR_NO_SUCH_FILE           ERROR_BASE"No such file found: %s\n"
#define ATOM_CT__CLI_ERR_UNKNOWN                ERROR_BASE"Unknown error code %d\n"

#define ATOM_CT__CLI_ERR_LEXERR                 ERROR_BASE"Lexer exited with code: %d. Unable to compile other files\n"

#define ATOM_CT__CLI_ERR_ARG_VALUE_INVALID      "Invalid argument type given to option `%s`, argument: `%s` expected type: `%s`, got: `%s`\n"
#define ATOM_CT__CLI_ERR_ARG_VALUE_OUT_OF_RANGE "Value provided is out of range when parsing type `%s`. Got: `%s`\n"
#define ATOM_CT__CLI_ERR_ARG_VALUE_UNKNOWN_ERR  "Unknown error occured when verifying argument `%s`, expected type: `%s`\n"

bool verify_args(int argc, char** argv);
void parse_args(int argc, char** argv);

void parse_flag(char* arg);
void parse_option(char *arg, char **argv, int argc, int *i);
Vector get_option_args(char** argv, int* argp, int argc);
void parse_file(char* file);

void parse_compile_ret(CompileRet ret);

#endif //ATOMIC_CLI_H
