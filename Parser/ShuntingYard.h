//
// Created by jamescoward on 25/01/2024.
//

#ifndef ATOMIC_SHUNTINGYARD_H
#define ATOMIC_SHUNTINGYARD_H

#include "Node.h"
#include "../Lexer/Tokens.h"

#include "../SharedIncludes/Queue.h"
#include "../SharedIncludes/Stack.h"

typedef enum ASS {
    LEFT,           // +-*/ etc
    RIGHT,          // ^    etc  --I ran out of examples

    ROTUND,         //ERROR
} ASS;

typedef struct ShuntRet {
    Node* expressionNode;
    uint tok_end_pos;
    uint err_code;
} ShuntRet;

typedef struct ShuntData {
    Token_vec* tokens;
    uint* t_pos;
} ShuntData;

#endif //ATOMIC_SHUNTINGYARD_H
