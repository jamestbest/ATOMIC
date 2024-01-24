//
// Created by jamescoward on 21/01/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "Tokens.h"
#include "Node.h"

//[[todo]] add Parserr
#include "Errors.h"

typedef struct NodeRet {
    Node node;
    uint retCode;
} NodeRet;

typedef struct TypeInformation {
    ATOM_CT__LEX_TYPES_ENUM type;
    uint16_t pointer_level;
} TypeInformation;

uint parse(const Token_vec* token_vec, const Vector* nodes);

#endif //PARSER_H
