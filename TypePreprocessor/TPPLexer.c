//
// Created by jamescoward on 29/07/2024.
//

#include "TPPLexer.h"

#include <stdlib.h>
#include <string.h>

#include "TPPLexerInternal.h"
#include "SharedIncludes/Helper_String.h"
#include "SharedIncludes/Messages.h"

ARRAY_ADD(TPPToken, TPPToken)

uint c_pos = 0;
const Buffer* c_line = NULL;

static bool setup = false;
TPPTokenArray tok_arr;
Vector const* operators_enum;
Vector const* types_enum;

static void idx_to(size_t new_idx);
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

    tok_arr = TPPToken_arr_construct(25);

    setup = true;
    return EXIT_SUCCESS;
}

void add_token(const TPPType type, void* data) {
    TPPToken token = {
        .type = type
    };

    if (type == IDENTIFIER || type == CUSTOM_OPERATOR) {
        token.data.str = data;
    } else if (type == NUMERIC) {
        token.data.numeric = *(long long*)data;
    }
    else token.data.pos = DBG_SPECIAL_SEQ;

    TPPToken_arr_add(&tok_arr, token);
}

void add_keyword(const KEYWORDS keyword) {
    const TPPToken tok = {
        .type= KEYWORD,
        .data.keyword= keyword
    };

    TPPToken_arr_add(&tok_arr, tok);
}

void add_keyvalue(const KEYVALUES kv) {
    const TPPToken tok= {
        .type= KEYVALUE,
        .data.keyvalue= kv
    };

    TPPToken_arr_add(&tok_arr, tok);
}

void add_symbol(const TPPType type) {
    const TPPToken tok = {
        .type= type,
        .data.str=NULL
    };

    TPPToken_arr_add(&tok_arr, tok);
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

char* get_identifier_end(char* start) {
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

int compare_strings_for_search(const void* stra, const void* strb) {
    return strcmp(stra, *(char**)strb);
}

void lex_identifier(char* start) {
    char* const end = get_identifier_end(start);
    const uint length = end - start;

    const char end_save = *end;
    *end = '\0';

    const char** kw_res= bsearch(start, KEYWORD_STRINGS, KEYWORD_COUNT, sizeof (KEYWORD_STRINGS[0]), compare_strings_for_search);
    const char** kv_res= bsearch(start, KEYVALUE_STRINGS, KEYVALUE_COUNT, sizeof (KEYVALUE_STRINGS[0]), compare_strings_for_search);

    *end = end_save;

    if (kw_res) {
        add_keyword(kw_res - KEYWORD_STRINGS);
    } else if (kv_res) {
        add_keyvalue(kv_res - KEYVALUE_STRINGS);
    } else {
        add_token(IDENTIFIER, copy_identifier(start, end));
    }

    c_pos += length;
}

bool is_non_op_symbol(char c) {
    return is_whitespace(c) || is_alph_numeric(c) || c == '\n' || c == '\0';
}

bool is_exact_symbol(const char* start, const char* symbol) {
    uint i = 0;
    while (symbol[i] != '\0') {
        if (start[i] != symbol[i]) return false;
        i++;
    }

    return is_non_op_symbol(start[i]);
}

bool consume_symbol(const char* start, const char* symbol) {
    if (is_exact_symbol(start, symbol)) {
        gorge(strlen(symbol));
    }
}

void tpplex_line(const Buffer* line_buffer) {
    if (!setup) {
        panic("lex_setup has not been called before lex_line\n");
    }

    c_pos = 0;
    c_line = line_buffer;

    if ((current() == '/' && peek() == '/')) {
        goto next;
    }

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

        const char* const c_ptr = current_ptr();
        if (consume_symbol(c_ptr, "=")) {
            add_symbol(EQUALITY);
        } else if (consume_symbol(c_ptr, ";")) {
            add_symbol(DELIMITER);
        } else if (consume_symbol(c_ptr, "||")) {
            add_symbol(OR);
        } else if (consume_symbol(c_ptr, "|")) {
            add_symbol(PIPE);
        } else if (consume_symbol(c_ptr, "&&")) {
            add_symbol(AND);
        } else if (consume_symbol(c_ptr, "<->")) {
            add_symbol(BIRROW);
        } else if (consume_symbol(c_ptr, "->")) {
            add_symbol(ARROW);
        } else if (is_digit(c)) {
            char* start = current_ptr();
            char* end;
            long long number = strtoll(start, &end, 10);
            // [[todo]] add error checking
            add_token(NUMERIC, &number);

            idx_to(end - start + c_pos);
        } else {
            const char* const start = current_ptr();
            char current_of_op;
            while (current_of_op= current(), !is_non_op_symbol(current_of_op)) {
                consume();
            }

            const char* const end = current_ptr();

            char* custom_op = copy_identifier(start, end);

            add_token(CUSTOM_OPERATOR, custom_op);
        }
    }

next:;
    TPPToken* temp;
    if (temp = arr_peek((Array*)&tok_arr), temp && temp->type != EOS) add_symbol(EOS);
}

TPPTokenArray tpplex_end() {
    if (!setup) {
        panic("lex_setup has not been called before lex_end");
    }

    setup = false;

    return tok_arr;
}

bool is_valid_index(const uint index) {
    return index < c_line->pos;
}

void idx_to(size_t new_idx) {
    if (!is_valid_index(new_idx)) assert(false);

    c_pos = new_idx;
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
    return peer(1);
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

const char* get_tpptoken_keyword_string(const KEYWORDS keyword) {
    return KEYWORD_STRINGS[keyword];
}

const char* get_tpptoken_keyvalue_string(const KEYVALUES kv) {
    return KEYVALUE_STRINGS[kv];
}

void print_tpptoken_type(const TPPType type) {
    printf("%s", get_tpptoken_type_string(type));
}

void print_tpptoken(const TPPToken* token) {
    print_tpptoken_type(token->type);

    const TPPType type = token->type;

    switch (type) {
        case IDENTIFIER:
        case CUSTOM_OPERATOR:
            printf("::%s", token->data.str);
            break;
        case NUMERIC:
            printf("::%lld", token->data.numeric);
            break;
        case KEYWORD:
            printf("::%s", get_tpptoken_keyword_string(token->data.keyword));
            break;
        case KEYVALUE:
            printf("::%s", get_tpptoken_keyvalue_string(token->data.keyvalue));
        default:
            break;
    }
}