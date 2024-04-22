//
// Created by jamescoward on 13/11/2023.
//

#ifndef ATOMIC_TOKENS_H
#define ATOMIC_TOKENS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../SharedIncludes/Vector.h"
#include "../SharedIncludes/Colours.h"
#include "../SharedIncludes/Helper_String.h"

typedef struct Arr {
    char** arr;
    uint elem_count;
} Arr;

//[[todo]] add `.`
typedef enum TokenType {
    TOKEN_INVALID,

    IDENTIFIER,
    KEYWORD,
    TYPE,

    LIT_STR,
    LIT_CHR,
    LIT_INT,
    LIT_FLOAT,
    LIT_BOOL,
    LIT_NAV,

    OP_BIN,
    OP_UN,
    OP_UN_PRE,
    OP_UN_POST,
    OP_TRINARY,
    OP_BIN_OR_UN,

    ASSIGN,
    ARITH_ASSIGN,

    BRACKET_OPEN,
    BRACKET_CLOSE,
    CURLY_OPEN,
    CURLY_CLOSE,
    PAREN_OPEN,
    PAREN_CLOSE,

    COMMA,
    TYPE_SET,   //:
    TYPE_IMPL_CAST, //::
    CARROT,

    COMMENT,

    DELIMITER,

    WS_S,                   //White space single/tab
    WS_T,
    NEWLINE,
    EOTS,                   //End of token stream
} TokenType;

typedef enum ATOM_CT__LEX_KEYWORD_ENUM {
    FOR, TO, DO,
    WHILE,
    FOREACH, IN, WITH,
    TIMES,
    IF, ELIF, ELSE,
    RET,
    FUNC, PROC, ENTRY,
    CONT, BRK,
    REC, HEADER,

    ATOM_CT__LEX_KEYWORD_ENUM_COUNT
} ATOM_CT__LEX_KEYWORD_ENUM;

typedef enum ATOM_CT__LEX_TYPES_ENUM {
    I1, I2, I4, I8,
    N1, N2, N4, N8,
    R4, R8, R10,
    Q4, Q8, Q16,
    STR,
    CHR,
    BOOL,
    NAV
} ATOM_CT__LEX_TYPES_ENUM;

typedef enum ATOM_CT__LEX_TYPES_GENERAL_ENUM {
    INTEGER,
    NATURAL,
    REAL,
    RATIONAL,
    STRING,
    CHAR,
    BOOLEAN,
    NOT_A_VALUE
} ATOM_CT__LEX_TYPES_GENERAL_ENUM;

typedef enum ATOM_CT__LEX_OP_IDENTIFIERS_ENUM {
    AND, OR, XOR,
    NOT,
    AS
} ATOM_CT__LEX_OP_IDENTIFIERS_ENUM;

typedef enum ATOM_CT__LEX_OPERATORS_ENUM {
    PLUS, MINUS,
    MULT, DIV,
    MOD, POW,

    BAND, BOR,

    SHL, SHR,

    ASS_PLUS, ASS_MINUS,
    ASS_MULT, ASS_DIV,
    ASS_MOD, ASS_POW,

    ASS_BAND, ASS_BOR,

    ASS_SHL, ASS_SHR,

    LAND, LOR, LXOR,
    BXOR,

    LNOT, BNOT,
    INC, DEC,

    QUESTION,

    AMPERSAND, // address of

    ASSIGNMENT,

    EQU, NEQ, LESS, MORE, LESSEQ, MOREEQ,

    DEREFERENCE,

    SWAP,
    RANGE,
    ARROW,
} ATOM_CT__LEX_OPERATORS_ENUM;

/* Should the base_tokens store their location?
 *   It would make printing their information easier
 *
 *   But it is not needed elsewhere in the compiler? Only really for printing the ast    ----AND ERRORS!
 *   Calculating the position of a single token would require looking at all previous base_tokens
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

typedef struct encodedType {
    unsigned short general_type : 16;
    unsigned short size : 16;
    unsigned short ptr_offset : 16;
    unsigned short enum_position : 16;
} encodedType;

//a base_tokens value is the |func mainfunction () : i4|
//                            ^----------^
//                            |           `line + capacity
//                            `line
typedef struct Token {
    TokenType type;
    union { //not sure if this is a good idea
        void* ptr;
        int64_t integer;
        uint64_t natural;
        long double real;
        encodedType type;
        int enum_pos;
    } data;

    Position pos;
} Token;

extern Arr ATOM_CT__LEX_OP_IDENTIFIERS;
extern char* ATOM_CT__LEX_OP_IDENTIFIERS_RAW[];
extern Arr ATOM_CT__LEX_KEYWORDS;
extern char* ATOM_CT__LEX_KEYWORDS_RAW[];
extern Arr ATOM_CT__LEX_TYPES;
extern char* ATOM_CT__LEX_TYPES_RAW[];
extern Arr ATOM_CT__LEX_TYPES_GENERAL;
extern char* ATOM_CT__LEX_TYPES_GENERAL_RAW[];
extern Arr ATOM_CT__LEX_CONS_IDENTIFIERS;
extern char* ATOM_CT__LEX_CONS_IDENTIFIERS_RAW[];
extern Arr ATOM_CT__LEX_OPERATORS;
extern char* ATOM_CT__LEX_OPERATORS_RAW[];

TokenType operator_to_type(ATOM_CT__LEX_OPERATORS_ENUM op);

const char* get_token_color(TokenType type);
const char* cons_token_type_colored(TokenType type);

bool type_needs_free(TokenType type);
bool is_whitespace_tkn(TokenType type);

int print_position(Position pos);
void print_token_value(Token* token);
void print_token(Token* token);

void consolidate(Token* base_token, Token* token_to_eat);

bool is_l_paren(Token* tok);
bool is_r_paren(Token* tok);
bool is_terminal(Token* tok);
bool is_operator(Token* tok);

VEC_PROTO(Token, Token)

#endif //ATOMIC_TOKENS_H
