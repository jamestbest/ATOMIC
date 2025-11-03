//
// Created by james on 25/12/23.
//

#ifndef ATOMIC_LEXER_H
#define ATOMIC_LEXER_H

#include "../Commons.h"


#include "Tokens.h"

#include "../SharedIncludes/Helper_String.h"
#include "../SharedIncludes/Helper_File.h"
#include "../SharedIncludes/Buffer.h"

#include "../SharedIncludes/Array.h"

#include "Lexerr.h"

ARRAY_PROTO(Token, token)

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

uint lex(FILE* file, tokenArray* tokens, Vector* lines);

void print_tokens(tokenArray* token_vec, bool include_ws, bool include_comments);
void print_verbose_tokens(tokenArray* tokens, Vector* lines, bool print_labels);

#endif //ATOMIC_LEXER_H
