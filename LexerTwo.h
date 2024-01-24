//
// Created by james on 25/12/23.
//

#ifndef ATOMIC_LEXERTWO_H
#define ATOMIC_LEXERTWO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "Tokens.h"
#include "LexErrors.h"

#include "SharedIncludes/Buffer.h"
#include "SharedIncludes/Helper_File.h"
#include "SharedIncludes/Helper_String.h"

typedef struct PosCharp{
    int arr_pos;
    char* next_char;
} PosCharp;

typedef struct ArrPosCharp {
    char** array;
    PosCharp posCharp;
} ArrPosCharp;

#define COMMENT_START_ASCII_CODE 172

typedef long long int llint;
typedef unsigned char uchar;

uint lex(FILE *file, Token_vec *token_vec, Vector *lines);

void print_tokens(Token_vec* token_vec, bool include_ws, bool include_comments);
void print_verbose_tokens(Token_vec* token_vec, Vector* lines, bool print_labels);

#endif //ATOMIC_LEXERTWO_H
