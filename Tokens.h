//
// Created by jamescoward on 13/11/2023.
//

#ifndef ATOMIC_TOKENS_H
#define ATOMIC_TOKENS_H

#include <stdlib.h>

#include "SharedIncludes/Vector.h"

typedef enum TokenType {
    IDENTIFIER,
    KEYWORD,

    LIT_STR,
    LIT_INT,
    LIT_FLOAT,
    LIT_BOOL,
    LIT_NAV,

    OP_BIN,
    OP_UN_PRE,
    OP_UN_POST,
    OP_TRINARY,

    BRACKET_OPEN,
    BRACKET_CLOSE,
    CURLY_OPEN,
    CURLY_CLOSE,
    PAREN_OPEN,
    PAREN_CLOSE,

    COMMA,

    WS_S,                   //White space single/tab
    WS_T,
    NEWLINE,
    EOTS,                   //End of token stream
} TokenType;

/* Should the tokens store their location?
 *   It would make printing their information easier
 *
 *   But it is not needed elsewhere in the compiler? Only really for printing the ast    ----AND ERRORS!
 *   Calculating the position of a single token would require looking at all previous tokens
 *   But calculating the position of each token one by one sequentially would be fairly cheap
 *   I would have to store the whitespace characters as well in order to calc the position instead of dropping '\t' and '\s'
 *
 *   [[maybe]] For now this will not store the positions
 */
//a tokens value is the |func mainfunction () : i4|
//                            ^----------^
//                            |           `line + size
//                            `line
typedef struct Token {
    TokenType type;
    char* start;
    size_t size;
} Token;

VEC_PROTO(Token, Token)

#endif //ATOMIC_TOKENS_H