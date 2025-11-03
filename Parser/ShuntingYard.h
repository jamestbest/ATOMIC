//
// Created by jamescoward on 25/01/2024.
//

#ifndef ATOMIC_SHUNTINGYARD_H
#define ATOMIC_SHUNTINGYARD_H

#include "../Commons.h"
#include "../Flags.h"
#include "../Lexer/Tokens.h"
#include "../SharedIncludes/Queue.h"
#include "../SharedIncludes/Stack.h"
#include "../SharedIncludes/Vector.h"
#include "Lexer/Lexer.h"
#include "Node.h"

typedef enum STATE {
    EXPECTING_START,        // The start of an expression can

    EXPECTING_LEFT,         // a left value is var, function call, UN_OP_PRE, or an open paren
    EXPECTING_CENTRE,       // a centre value is one after a left value e.g. operator, or close paren
} STATE;

typedef enum ASS {
    LEFT,           // +-*/ etc
    RIGHT,          // ^    etc  --I ran out of examples

    ROTUND,         //ERROR
} ASS;

typedef struct ShuntRet {
    Node* expressionNode;
    int tok_end_pos;
    uint err_code;
} ShuntRet;

typedef struct ShuntData {
    const tokenArray* tokens;
    uint* t_pos;
} ShuntData;

ShuntRet shunt(const tokenArray* tokens, uint t_pos, bool ignoreTrailingParens);

#endif //ATOMIC_SHUNTINGYARD_H
