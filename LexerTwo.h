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
    char* last_char;
} PosCharp;

#define COMMENT_START_ASCII_CODE 172

typedef long long int llint;
typedef unsigned char uchar;

uint lex(FILE *file, Token_vec *token_vec, Vector *lines);
uint lex_line(Buffer* line);

PosCharp word_in_arr(const char* word, Arr arr);
void lex_word(void);
void lex_identifier(void);
uint lex_number(void);
int lex_comment(void);
int lex_multiline_comment(void);

Token create_token(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col);
Token create_multiline_token(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col, uint32_t start_line);
Token construct_multiline_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col, uint32_t start_line);
Token create_token_and_term(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col);
Token construct_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col);
Token create_simple_token(TokenType type, uint32_t start_col, uint32_t end_col);
void add_token(Token t);

void print_tokens(Token_vec* token_vec);
void print_verbose_tokens(Token_vec* token_vec, Vector* lines, bool print_labels);

uint32_t current_char(void);
uint32_t peek(void);
char* consume(void);
char* gourge(uint amount);
int carridge_next_line(void);
char* consume_with_carridge(void);
void update_line_count(void);
uint32_t consume_utf_char(char *start, bool consume, char** next_char);

void highlight_line_err(Position pos, const char* line);
void highlight_current_line_err(Position pos);
char* lexerr_process_char(char a, char buff[2]);
void lexwarn(Lexwarns warnCode, Position pos, ...);
uint lexerr(Lexerrors errorCode, Position pos, ...);

#endif //ATOMIC_LEXERTWO_H
