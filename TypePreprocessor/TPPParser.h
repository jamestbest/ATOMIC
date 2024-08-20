//
// Created by james on 01/08/24.
//

#ifndef ATOMIC_TPPPARSER_H
#define ATOMIC_TPPPARSER_H

#include "../SharedIncludes/Array.h"
#include "TPPLexer.h"

void tpp_parse(Array tokens, const Vector* types_enums, const Vector* operator_enums);

typedef enum {
    ROOT,
    STMNT_ALIAS,
    STMNT_OPERATOR,
    STMNT_COERCE
} TPPNodeType;

typedef struct TPPNode {
    TPPNodeType type;
    TPPToken token;
    Vector children;
} TPPNode;

#endif // ATOMIC_TPPPARSER_H
