//
// Created by jamescoward on 21/01/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "../Commons.h"

#include "../Lexer/Tokens.h"
#include "Node.h"
#include "ShuntingYard.h"

#include "Parserr.h"

typedef struct TypeInformation {
    ATOM_CT__LEX_TYPES_GENERAL_ENUM type;
    uint16_t pointer_level;
} TypeInformation;

NodeRet parse(const Array* token_vec, const Vector* lines);

#endif //PARSER_H
