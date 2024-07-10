//
// Created by jamescoward on 24/01/2024.
//

#ifndef ATOMIC_PARSERR_H
#define ATOMIC_PARSERR_H

#include "../Commons.h"
#include "../Errors.h"
#include "Node.h"

typedef enum ParsErrors {
    PARSERR_BLOCK_MISSING_BRACE,
    PARSERR_SUB_CALL_PARSE_ERROR,
    PARSERR_SUB_CALL_EXPECTED_GOT_EXPR,
    PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT,
    PARSERR_SUB_STATEMENT_ERROR_IN_BODY,
    PARSERR_UNEXPECTED_TOKEN_STATEMENT_START,

    PARSERR_SA_ALREADY_DEFINED,
    PARSERR_SA_NOT_IN_SCOPE,
} ParsErrors;

typedef enum ParseWarns {
    PARSEWARN_SA_SHADOWS_PREVIOUS_DECL,
} ParseWarns;

extern const Vector* plines;

NodeRet parserr(ParsErrors errorCode, Token* parent_token, Token* issue_token, ...);
NodeRet parsewarn(const ParseWarns warningCode, Token* parent_token, Token* issue_token, ...);

//ERRORS
#define ATOM_CT__PARSERR_BLOCK_MISSING_BRACE "Block did not parse ending with curly brace." \
            "This may be because there is an error in the statement block or because there is a missing brace"

#define ATOM_CT__PARSERR_SUB_CALL_PARSE_ERROR "Subroutine call was detected but was not parsed correctly"

#define ATOM_CT__PARSERR_SUB_CALL_EXPECTED_GOT_EXPR "Started parsing a subroutine call but got a generic expression." \
            "Generic expressions are not allowed as statements"

#define ATOM_CT__PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT "Unexpected token found in parsing of an identifier statement.\n" \
            "Valid next tokens are type set for variable declaration `:`/`::`, subroutine calls with `(`, variable assignments with `=`/`+=` etc, or unary operators `++`"

#define ATOM_CT__PARSERR_SUB_STATEMENT_ERROR_IN_BODY "Error occurred when parsing subroutine body."

#define ATOM_CT__PARSERR_UNEXPECTED_TOKEN_STATEMENT_START "Unexpected token found while parsing for a statement."

#define ATOM_CT__PARSERR_SA_ALREADY_DEFINED "Declaration of '%s' already exists\n"

#define ATOM_CT__PARSERR_SA_NOT_IN_SCOPE "Identifier '%s' not in accessible scope\n"

//WARNINGS

#define ATOM_CT__PARSEWARN_SA_SHADOWS_PREVIOUS_DECL "Declaration of '%s' shadows previous declaration\n"

#endif // ATOMIC_PARSERR_H
