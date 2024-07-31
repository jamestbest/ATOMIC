//
// Created by jamescoward on 02/06/2024.
//

#include "Parserr.h"

NodeRet parsewarn(const ParseWarns warningCode, Token* parent_token, Token* issue_token, ...) {
    putz(C_MGN"PARSEWARNING"C_RST": ");

    switch (warningCode) {
        case PARSEWARN_SA_SHADOWS_PREVIOUS_DECL:
            printf(ATOM_CT__PARSEWARN_SA_SHADOWS_PREVIOUS_DECL, issue_token->data.ptr);
            break;
        default:
            assert(false);
    }

    highlight_line_start_and_error(parent_token, issue_token, plines);

    return (NodeRet){NULL, FAIL};
}

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
        case PARSERR_SA_ALREADY_DEFINED:
            printf(ATOM_CT__PARSERR_SA_ALREADY_DEFINED, issue_token->data.ptr);
            break;
        case PARSERR_SA_NOT_IN_SCOPE:
            printf(ATOM_CT__PARSERR_SA_NOT_IN_SCOPE, issue_token->data.ptr);
            break;
        default:
            assert(false);
    }

    highlight_line_start_and_error(parent_token, issue_token, plines);

    newline();

    return (NodeRet){NULL, FAIL};
}
