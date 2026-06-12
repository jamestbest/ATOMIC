//
// Created by jamescoward on 11/06/2026.
//

#ifndef ATOMIC_PARSERR_H
#define ATOMIC_PARSERR_H

#include "Lexer/Tokens.h"
#include "Lexer/Tokerr.h"
#include "Parser2/Node.h"

NodeRet parser_error(const char* message, const Token* parent_tok, const Token* issue_tok, ...);
NodeRet parser_unexpected(const char* context, TokenType expected_type, const Token* issue_token, ...);

typedef enum {
    PFT_REMOVAL,
    PFT_ADDITION,
    PFT_REPLACE
} PARSER_FIX_TYPE;

typedef enum {
    PFO_AFTER,
    PFO_BEFORE,
    PFO_NONE
} PARSER_FIX_OFFSET;

void parser_fix(
    const Token* base_token, PARSER_FIX_OFFSET offset,
    PARSER_FIX_TYPE type,
    const char* addition,
    const char* reason
);
void parser_fix_or();

#endif //ATOMIC_PARSERR_H

