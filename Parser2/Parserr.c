//
// Created by jamescoward on 11/06/2026.
//

#include "Parserr.h"

#include "Errors.h"
#include "SharedIncludes/Buffer.h"

extern const Vector* plines;

static bool has_removal(const PARSER_FIX_TYPE type) {
    switch (type) {
        case PFT_REPLACE:
        case PFT_REMOVAL:
            return true;
        case PFT_ADDITION:
            return false;
        default:
            assert(false);
    }
}

static bool has_addition(const PARSER_FIX_TYPE type) {
    switch (type) {
        case PFT_REPLACE:
        case PFT_ADDITION:
            return true;
        case PFT_REMOVAL:
            return false;
        default:
            assert(false);
    }
}

static void parser_fix_(
    const Token* base_token, PARSER_FIX_OFFSET offset,
    PARSER_FIX_TYPE type,
    const char* addition,
    const char* reason
) {
    const char* line= vector_get_unsafe(plines, base_token->pos.start_line);
    const size_t line_len= strlen(line) + 1;
    const size_t add_len= strlen(addition) + 1;

    Buffer buffer= buffer_create(line_len + add_len);
    buffer_concat(&buffer, line);

    const size_t insert_pos= offset == PFO_BEFORE ? base_token->pos.start_col - 1 : base_token->pos.end_col;
    buffer_insert(&buffer, addition, insert_pos);

    Position pos= base_token->pos;
    pos.start_col= insert_pos;
    pos.end_col= insert_pos + add_len;

    char* n_line= buffer.data;

    switch (type) {
        case PFT_ADDITION: {
            highlight_line_multiple(n_line, pos, (char*[]){C_GRN, NULL}, pos);
            break;
        }
        case PFT_REMOVAL: {
            highlight_line_err(base_token->pos, n_line, -1);
            break;
        }
        case PFT_REPLACE: {
            char** colours;
            if (offset == PFO_BEFORE) {
                colours= (char*[]){C_GRN, C_RED, NULL};
                highlight_line_multiple(n_line, pos, colours, pos, base_token->pos);
            } else {
                colours= (char*[]){C_RED, C_GRN, NULL};
                highlight_line_multiple(n_line, pos, colours, base_token->pos, pos);
            }
        }
    }
}

void parser_fix(
    const Token* base_token, PARSER_FIX_OFFSET offset,
    PARSER_FIX_TYPE type,
    const char* addition,
    const char* reason
) {
    parser_fix_(base_token, offset, type, addition, reason);
}

void parser_fix_or() {
    printf(C_MGN" OR \n"C_RST);
}

NodeRet parser_unexpected(const char* context, TokenType expected_type, const Token* issue_token, ...) {
    va_list args;

    printf(C_RED"PARSERROR"C_RST": Unexpected token, expected '%s' in ",
        TOKEN_TYPE_STRS[expected_type]
    );

    va_start(args, issue_token);
    vprintf(context, args);
    va_end(args);

    highlight_line_start_and_error(NULL, issue_token, plines);

    return PARSE_FAIL;
}

NodeRet parser_error(const char* message, const Token* parent_tok, const Token* issue_tok, ...) {
    va_list args;

    putz(C_RED"PARSERROR"C_RST": ");

    va_start(args, issue_tok);
    vprintf(message, args);
    va_end(args);

    highlight_line_start_and_error(parent_tok, issue_tok, plines);

    newline();

    return PARSE_FAIL;
}