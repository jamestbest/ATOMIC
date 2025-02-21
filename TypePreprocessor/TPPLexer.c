//
// Created by jamescoward on 29/07/2024.
//

#include "TPPLexer.h"

#include "TPPLexerInternal.h"
#include "../SharedIncludes/Helper_String.h"
#include "../SharedIncludes/Messages.h"

ARRAY_ADD(TPPToken, TPPToken)

uint c_pos = 0;
const Buffer* c_line = NULL;

bool setup = false;
Array tok_arr;
Vector const* operators_enum;
Vector const* types_enum;

static char consume();
static void gorge(size_t amount);
static char peek();
static char peer(size_t amount);
static char current();
static char* current_ptr();
static char expect(char e);
static char expect_peek(char e);

/** STRUCTURE OF TYPES FILE
 *
 *  The types file contains information on the following
 *   - The types in ATOM along with their symbols
 *   - Aliased types (Their name + types)
 *   - The operators in ATOM along with their symbol
 *   - Type coercion
 *   - Operator operand types
 */

/** FORMAT OF TYPES FILE (In order as above ^)
 *
 *  TYPES
 *  <INTERNAL TYPE NAME> (<TYPE NAME>| PREFIX <TYPE PREFIX> OVER [NUMBER])
 *  INTEGER PREFIX i OVER 1 2 4 8
 *  CHAR chr
 *
 *  ALIASES
 *  <ALIAS NAME> = [TYPES] -- NOTE: Types cannot be aliases (no nested aliasing)
 *  NUMERIC = INTEGER NATURAL REAL RATIONAL
 *
 *  OPERATORS
 *  <OPERATOR NAME> <OPERATOR SYMBOL>
 *  MULT *
 *
 *  COERCIONS
 *  <TYPES> (-> | <->) <TYPES>
 *    WHERE TYPES = TYPE | (TYPE `|` TYPES)
 *  CHAR <-> INTEGER | NATURAL
 *
 *  OPERANDS
 *  <OPERATOR> = <TYPE LIST>
 *    WHERE TYPE LIST = <TYPE SET> | <TYPE SET>; <TYPE LIST>
 *    WHERE TYPE SET = <TYPE> | (<TYPES> (`||` | `&&`) <TYPES>)
 *  PLUS = NUMERIC; ALPHABETICAL; POINTER || INTEGER|NATURAL|POINTER;
 */


uint tpplex_setup(Vector const* type_enum, Vector const*operator_enum) {
    types_enum = type_enum;
    operators_enum = operator_enum;

    tok_arr = arr_construct(sizeof (TPPToken), 25);

    setup = true;
    return EXIT_SUCCESS;
}

void add_token(const TPPType type, void* data) {
    TPPToken token = {
        .type = type
    };

    if (type == IDENTIFIER) {
        token.data.str = data;
    } else if (type == NUMERIC) {
        token.data.numeric = *(long long*)data;
    }
    else token.data.pos = DBG_SPECIAL_SEQ;

    TPPToken_arr_add(&tok_arr, token);
}

void add_keyword(const TPPType type) {
    add_token(type, NULL);
}

char* copy_identifier(const char* start, const char* end) {
    const uint length = end - start;

    char* res = malloc(sizeof (char) * length + 1);
    memcpy(res, start, length);
    *(res + length) = '\0';

    return res;
}

bool is_valid_idenitifier_tail(const char character) {
    return is_alph_numeric(character) || character == '_';
}

const char* get_identifier_end(const char* start) {
    while (is_valid_idenitifier_tail(*start)) start++;

    return start;
}

uint get_vector_position(const Vector* vector, const char* match, const uint length) {
    for (uint i = 0; i < vector->pos; ++i) {
        const char* match_check = vector_get_unsafe(vector, i);

        if (strncmp(match, match_check, length) == 0) return i;
    }

    return -1;
}

bool matches_keyword(const char keyword[], const uint length, const char* start) {
    return strncmp(start, keyword, length) == 0;
}

void lex_identifier(const char* start) {
    const char* end = get_identifier_end(start);
    const uint length = end - start;

    // I actually *really* hate this, why can I not make a char[][] in C!? it's really fucking ridiculous
    if (matches_keyword(KEYWORD_ALIASES, length, start)) {
        add_keyword(ALIASES);
    } else if (matches_keyword(KEYWORD_TYPES, length, start)) {
        add_keyword(TYPES);
    } else if (matches_keyword(KEYWORD_OPERANDS, length, start)) {
        add_keyword(OPERANDS);
    } else if (matches_keyword(KEYWORD_COERCIONS, length, start)) {
        add_keyword(COERCIONS);
    } else if (matches_keyword(KEYWORD_OPERATORS, length, start)) {
        add_keyword(OPERATORS);
    }  else {
        add_token(IDENTIFIER, copy_identifier(start, end));
    }

    c_pos += length;
}

void tpplex_line(const Buffer* line_buffer) {
    if (!setup) {
        panic("lex_setup has not been called before lex_line\n");
    }

    c_pos = 0;
    c_line = line_buffer;

    char c;
    while (c = current(), c != '\0') {
        if (is_alph(c)) {
            lex_identifier(&c_line->data[c_pos]);
            continue;
        }

        if (is_whitespace(c) || is_newline(c)) {
            consume();
            continue;
        }

        if (c == '=') {
            add_keyword(EQUALITY);
        } else if (c == ';') {
            add_keyword(DELIMITER);
        } else if (c == '|') {
            if (expect_peek('|')) {
                add_keyword(OR);
            } else {
                add_keyword(PIPE);
            }
        } else if (c == '&') {
            if (expect_peek('&')) {
                add_keyword(AND);
            } else error("`&` is not a valid symbol, `&&` is. Peek is `%c`\n", peek());
        } else if (c == '<') {
            const bool join_next = peek() == '-' && peer(2) == '>';

            if (!join_next) error("`<` is not a valid symbol, `<->` is. Peek is `%c`\n", peek());
            else {
                add_keyword(BIRROW);
                gorge(2);
            }
        } else if (c == '-') {
            if (expect_peek('>')) {
                add_keyword(ARROW);
            } else error("`-` is not a valid symbol, `->` is. Peek is `%c`\n", peek());
        } else if (is_digit(c)) {
            char* end;
            long long number = strtoll(current_ptr(), &end, 10);
            // [[todo]] add error checking
            add_token(NUMERIC, &number);
        } else {
            // assume it to be an operator symbol and lex to whitespace
            error("Invalid character `%c` provided, unable to lex to token\n", c);
        }

        consume();
    }

    TPPToken* temp;
    if (temp = arr_peek(&tok_arr), temp && temp->type != EOS) add_keyword(EOS);
}

Array tpplex_end() {
    if (!setup) {
        panic("lex_setup has not been called before lex_end");
    }

    setup = false;

    return tok_arr;
}

bool is_valid_index(const uint index) {
    return index < c_line->pos;
}

// Only supports ascii
char consume() {
    if (!is_valid_index(c_pos)) return '\0';

    return c_line->data[c_pos++];
}

void gorge(const size_t amount) {
    for (uint i= 0; i < amount; ++i) {
        if (consume() == '\0') break;
    }
}

char peek() {
    if (!is_valid_index(c_pos + 1)) return '\0';

    return c_line->data[c_pos + 1];
}

char peer(const size_t amount) {
    if (!is_valid_index(c_pos + amount)) return '\0';

    return c_line->data[c_pos + amount];
}

char current() {
    if (!is_valid_index(c_pos)) return '\0';

    return c_line->data[c_pos];
}

char* current_ptr() {
    if (!is_valid_index(c_pos)) return NULL;

    return &c_line->data[c_pos];
}

char expect(const char e) {
    if (current() == e) {
        return consume();
    }

    return '\0';
}

// this is a bit of a figglesome function; it is basically only to be used when checking
//  if the peeked token is part of the same operator e.g. `||` It consumes once, expecting
//  the calling function to consume the ending part.
char expect_peek(const char e) {
    if (peek() == e) {
        consume();
        return e;
    }

    return '\0';
}

const char* get_tpptoken_type_string(const TPPType type) {
    return TPPTypesStrings[type];
}

void print_tpptoken_type(const TPPType type) {
    printf("%s", get_tpptoken_type_string(type));
}

void print_tpptoken(const TPPToken* token) {
    print_tpptoken_type(token->type);

    const TPPType type = token->type;

    if (type == IDENTIFIER) {
        printf("::%s", token->data.str);
    } else if (type == NUMERIC) {
        printf("::%lld", token->data.numeric);
    }
}