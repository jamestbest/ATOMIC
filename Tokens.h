//
// Created by jamescoward on 13/11/2023.
//

#ifndef ATOMIC_TOKENS_H
#define ATOMIC_TOKENS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Colours.h"
#include "SharedIncludes/Helper_String.h"

typedef struct Arr {
    char** arr;
    uint size;
} Arr;

typedef enum TokenType {
    INVALID,

    IDENTIFIER,
    KEYWORD,
    TYPE,

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
    TYPE_SET,
    TYPE_INFER,

    COMMENT,

    DELIMITER,

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

typedef struct Position {
    uint32_t start_line;
    uint32_t start_col;
    uint32_t end_line;
    uint32_t end_col;
} Position;

//a tokens value is the |func mainfunction () : i4|
//                            ^----------^
//                            |           `line + size
//                            `line
typedef struct Token {
    TokenType type;
    void* data; //now going to be some data, could point to a string, or a number, or a single character

    Position pos;
} Token;

extern Arr ATOM_CT__LEX_OP_IDENTIFIERS;
extern char* ATOM_CT__LEX_OP_IDENTIFIERS_RAW[];
extern Arr ATOM_CT__LEX_KEYWORDS;
extern char* ATOM_CT__LEX_KEYWORDS_RAW[];
extern Arr ATOM_CT__LEX_TYPES;
extern char* ATOM_CT__LEX_TYPES_RAW[];
extern Arr ATOM_CT__LEX_CONS_IDENTIFIERS;
extern char* ATOM_CT__LEX_CONS_IDENTIFIERS_RAW[];


const char* get_token_color(TokenType type);
const char* cons_token_type_colored(TokenType type);

int print_position(Position pos);
void print_token_value(Token* token);
void print_token(Token* token);

VEC_PROTO(Token, Token)

#endif //ATOMIC_TOKENS_H
