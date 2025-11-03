//
// Created by jamescoward on 13/11/2023.
//

#ifndef ATOMIC_TOKENS_H
#define ATOMIC_TOKENS_H

#include "Commons.h"

#include "TypePreprocessor/output/enum-out.h"

#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Helper_String.h"
#include "SharedIncludes/Colours.h"

typedef struct Arr {
    const char** arr;
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

    EXPR_BIN,
    EXPR_UN,
    EXPR_UN_PRE,
    EXPR_UN_POST,
    EXPR_TRINARY,

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

    BACKSLASH,      // `\` -- for removing `\n`

    WS_S,                   //White space single/tab
    WS_T,
    NEWLINE,
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


typedef enum ATOM_CT__LEX_OP_IDENTIFIERS_ENUM {
    AND, OR, XOR,
    NOT,
    AS
} ATOM_CT__LEX_OP_IDENTIFIERS_ENUM;

/* Should the base_tokens store their location?
 *   It would make printing their information easier
 *
 *   But it is not needed elsewhere in the compiler? Only really for printing the ast    ----AND ERRORS!
 *   Calculating the position of a single token would require looking at all previous base_tokens
 *   But calculating the position of each token one by one sequentially would be fairly cheap
 *   I would have to store the whitespace characters as well in order to calc the position instead of dropping '\t' and '\s'
 */

typedef struct Position {
    uint32_t start_line;
    uint32_t start_col;
    uint32_t end_line;
    uint32_t end_col;
} Position;

// typedef struct encodedType {
//     uint64_t general_type: 16;
//     uint64_t size: 16;
//     uint64_t ptr_offset: 16;
//     uint64_t enum_position: 16;
// } encodedType;

//a base_tokens value is the |func mainfunction () : i4|
//                            ^----------^
//                            |           `line + capacity
//                            `line
typedef struct Token {
    TokenType type;
    union { //not sure if this is a good idea
        char* ptr;
        int64_t integer;
        uint64_t natural;
        long double real;
        encodedType type;
        int enum_pos;
    } data;

    Position pos;
} Token;

extern const char* ATOM_CT__LEX_NAV;

extern Arr ATOM_CT__LEX_KEYWORDS;
extern const char* ATOM_CT__LEX_KEYWORDS_RAW[];
extern Arr ATOM_CT__LEX_TYPES;
extern Arr ATOM_CT__LEX_TYPES_GENERAL;
extern Arr ATOM_CT__LEX_TYPES_GENERAL_SMALL;
extern Arr ATOM_CT__LEX_LIT_BOOLS;
extern const char* ATOM_CT__LEX_LIT_BOOLS_RAW[];
extern Arr ATOM_CT__LEX_OPERATORS;

TokenType operator_to_type(ATOM_CT__LEX_OPERATORS_ENUM op);

const char* get_token_color(TokenType type);
const char* cons_token_type_colored(TokenType type);

bool type_needs_free(TokenType type);
bool is_whitespace_tkn(TokenType type);

uint length_of_number_printout(const long long int n);
uint length_of_position_printout(Position pos);
int print_position(Position pos);
void print_token_value(const Token* token);
void print_token(const Token* token);
void print_token_ln(Token* token);

const char* get_token_type_string(TokenType type);

void consolidate(Token* base_token, Token* token_to_eat);

bool is_l_paren(Token* tok);
bool is_r_paren(Token* tok);
bool is_terminal(Token* tok);
bool is_arith_operator(Token* tok);
bool is_assigning_operator(Token* tok);
bool is_any_operator(Token* tok);

bool is_l_square_bracket(Token* tok);
bool is_r_square_bracket(Token* tok);
bool is_square_bracket(Token* tok);

#endif //ATOMIC_TOKENS_H
