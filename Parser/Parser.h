//
// Created by jamescoward on 21/01/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "../Lexer/Tokens.h"
#include "Node.h"

#include "ParsErrors.h"

typedef struct TypeInformation {
    ATOM_CT__LEX_TYPES_GENERAL_ENUM type;
    uint16_t pointer_level;
} TypeInformation;

uint parse(const Token_vec *token_vec, const Node_vec *nodes, const Vector *lines);

#endif //PARSER_H
