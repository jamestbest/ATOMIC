//
// Created by jamescoward on 06/11/2023.
//

#ifndef ATOMIC_CLI_H
#define ATOMIC_CLI_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Helper_String.h"
#include "Errors.h"
#include "Flags.h"
#include "Compiler.h"

typedef unsigned int uint;
typedef long long int llint;

#define ATOM_CT__CLI_DEFAULT_OUT                "out.a"
#define ATOM_CT__CLI_DEFAULT_FILE_BUFF_SIZE     4

bool verify_args(int argc, char** argv);
void parse_args(int argc, char** argv);

void parse_flag(char* arg);
void parse_option(char* arg, uint arg_len, char** argv, int argc, int* i);
charp_vec get_option_args(char** argv, int* argp, int argc);
void parse_file(char* file);

#endif //ATOMIC_CLI_H
