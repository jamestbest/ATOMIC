//
// Created by jamescoward on 02/06/2024.
//

#include "parserr.h"

NodeRet parserr(const ParsErrors errorCode, Token* parent_token, Token* issue_token, ...) {
    putz(C_RED"PARSERROR"C_RST": ");

    switch (errorCode) {
        case PARSERR_BLOCK_MISSING_BRACE:
            puts(ATOM_CT__PARSERR_BLOCK_MISSING_BRACE);
            break;
        case PARSERR_SUB_CALL_PARSE_ERROR:
            puts(ATOM_CT__PARSERR_SUB_CALL_PARSE_ERROR);
            break;
        case PARSERR_SUB_CALL_EXPECTED_GOT_EXPR:
            puts(ATOM_CT__PARSERR_SUB_CALL_EXPECTED_GOT_EXPR);
            break;
        case PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT:
            puts(ATOM_CT__PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT);
            break;
        case PARSERR_SUB_STATEMENT_ERROR_IN_BODY:
            puts(ATOM_CT__PARSERR_SUB_STATEMENT_ERROR_IN_BODY);
            break;
        case PARSERR_UNEXPECTED_TOKEN_STATEMENT_START:
            puts(ATOM_CT__PARSERR_UNEXPECTED_TOKEN_STATEMENT_START);
            break;
        default:
            assert(false);
    }

    highlight_line_start_and_error(parent_token, issue_token, plines);

    return (NodeRet){NULL, FAIL};
}
