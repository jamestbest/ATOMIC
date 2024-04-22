//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_COMPILER_H
#define ATOMIC_COMPILER_H

#include <stdio.h>

#include "SharedIncludes/Vector.h"
#include "Flags.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Lexer/Tokens.h"

#define SUCCESS 0
#define ERR_NO_SUCH_FILE 1

#define BUFF_SIZE 100

VEC_PROTO(char*, charp)

typedef struct CompileRet {
    int code;
    const char* info;
} CompileRet;

CompileRet compile(const char* entry_point, const char* out_format, const char* cwd, charp_vec files);
CompileRet compile_file(const char* entry_point, const char* out_format, FILE* fp);

void print_tokens_with_flag_check(Token_vec* tokens, Vector* lines);

void free_tokens(Token_vec* tokens);
void free_nodes(Node_vec *nodes);

#endif //ATOMIC_COMPILER_H
