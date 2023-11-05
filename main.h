//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_MAIN_H
#define ATOMIC_MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "helper.h"
#include "FLAGS.h"

#include "Lexer.h"

typedef unsigned int uint;

bool verifyArgs(int argc, char** argv);
void parseArgs(int argc, char** argv);

void parseFlag(char* arg, uint arg_len);
void parseOption(char* option, uint option_len, char** args, int arg_index);
void parseFile(char* filename, uint filename_len);

#endif //ATOMIC_MAIN_H