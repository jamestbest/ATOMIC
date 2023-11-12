//
// Created by james on 28/10/23.
//

#ifndef ATOMIC_LEXER_H
#define ATOMIC_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Helper_File.h"

#define BUFF_SIZE 100

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

void line_to_tokens(char_vec* line, Token_vec* tokens);

#endif //ATOMIC_LEXER_H