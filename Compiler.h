//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_COMPILER_H
#define ATOMIC_COMPILER_H

#include "Commons.h"

#include "SharedIncludes/Vector.h"
#include "Flags.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Lexer/Tokens.h"

#define SUCCESS 0
#define ERR_NO_SUCH_FILE 1

#define BUFF_SIZE 100

typedef struct CompileRet {
    int code;
    const char* info;
} CompileRet;

CompileRet compile(const char* entry_point, const char* out_format, const char* cwd, Vector files);
CompileRet compile_file(const char* entry_point, const char* out_format, FILE* fp);

void print_tokens_with_flag_check(Array* tokens, Vector* lines, const char* print_header);
void print_ast_with_flag_check(Node* tl_node);

#endif //ATOMIC_COMPILER_H
