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

typedef unsigned int uint;

bool verifyArgs(int argc, char** argv);
void parseArgs(int argc, char** argv);

void parseFlag(char* arg);
void parseOption(char* option, uint option_len, char** args, int arg_index);
void parseFile(char* filename, uint filename_len);

#endif //ATOMIC_CLI_H
