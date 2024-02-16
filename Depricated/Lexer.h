//
// Created by james on 28/10/23.
//

#ifndef ATOMIC_LEXER_H
#define ATOMIC_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "../SharedIncludes/Vector.h"
#include "../SharedIncludes/Buffer.h"
#include "../SharedIncludes/Helper_File.h"
#include "../Lexer/LexErrors.h"

#include "../Lexer/Tokens.h"

#define BUFF_SIZE 100

typedef long long int llint;

typedef struct LexRet {
    uint errCount;
    uint warnCount;
} LexRet;

typedef struct PosCharp {
    int pos;
    char* last;
} PosCharp;

LexRet lex(FILE *file, Token_vec *token_vec, Vector *lines);

void line_to_tokens(Buffer* line, Token_vec* tokens);
Token lex_num(void);
Token lex_word(void);
Token lex_op_identifier(PosCharp position);

char* consume(void);

void printToken(Token* token);
void printPosition(uint startLine, uint startCol, uint endLine, uint endCol);

void add_token(TokenType type, void* data, size_t d_size, uint d_length, Token_vec* token_vector);
Token create_token(TokenType type, void* data, size_t d_size, uint line, uint start_col, uint end_col);

void lexError(uint startLine, uint startCol, uint endLine, uint endCol, const char* error, ...);
void lexWarn(uint startLine, uint startCol, uint endLine, uint endCol, const char* error, ...);

PosCharp is_in(const char* start, Arr arr);

void print_token(Token* t);
void print_tokens(Token_vec* token_vec);

#endif //ATOMIC_LEXER_H
