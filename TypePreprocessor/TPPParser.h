//
// Created by james on 01/08/24.
//

#ifndef ATOMIC_TPPPARSER_H
#define ATOMIC_TPPPARSER_H

#include "SharedIncludes/Array.h"
#include "TPPLexer.h"

#include <Errors.h>

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

errcode parse_types_file(TPPTokenArray tokens);

size_t type_matrix_bytes();

#endif // ATOMIC_TPPPARSER_H
