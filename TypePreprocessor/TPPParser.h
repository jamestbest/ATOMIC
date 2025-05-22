//
// Created by james on 01/08/24.
//

#ifndef ATOMIC_TPPPARSER_H
#define ATOMIC_TPPPARSER_H

#include "SharedIncludes/Array.h"
#include "TPPLexer.h"

typedef enum {
    ROOT,
    STMNT_TYPEFIX,
    STMNT_TYPE,
    STMNT_OPERATOR,
    STMNT_ALIAS,
    STMNT_COERCE,
    STMNT_OPERANDS
} TPPNodeType;

typedef enum sections {
    SECTION_NONE,
    SECTION_TYPEFIX,
    SECTION_TYPES,
    SECTION_OPERATORS,
    SECTION_ALIASES,
    SECTION_COERCIONS,
    SECTION_OPERANDS,
    SECTION_COUNT
} sections;

typedef struct TPPNode {
    TPPNodeType type;
    TPPToken token;
    Vector children;
} TPPNode;

TPPNode* tpp_parse(Array tokens);

#endif // ATOMIC_TPPPARSER_H
