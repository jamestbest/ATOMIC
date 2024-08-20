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
Vector operators_enum;
Vector types_enum;

static char consume();
static char peek();
static char current();

uint tpplex_setup(const Vector* type_enum, const Vector* operator_enum) {
    types_enum = *type_enum;
    operators_enum = *operator_enum;

    tok_arr = arr_construct(sizeof (TPPToken), 25);

    setup = true;
    return EXIT_SUCCESS;
}

void add_token(const TPPType type, void* data) {
    TPPToken token = {
        .type = type
    };

    if (type == OPERATOR || type == TYPE) {
        token.data.pos = *(uint*)data;
    }
    else if (type == IDENTIFIER) {
        token.data.str = data;
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

void lex_identifier(const char* start) {
    const char* end = get_identifier_end(start);
    const uint length = end - start;

    uint pos;
    if (COMP_STR_LEN(KEYWORD_ALIAS) == length && strncmp(start, KEYWORD_ALIAS, length) == 0) {
        add_keyword(ALIAS);
    } else if (pos = get_vector_position(&types_enum, start, length), pos != (uint)-1) { // if is TYPE
        add_token(TYPE, &pos);
    } else if (pos = get_vector_position(&operators_enum, start, length), pos != (uint)-1) { // if is operator
        add_token(OPERATOR, &pos);
    } else { // if is identifier ¬ probably is an alias otherwise an error
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
            goto skip_consume;
        }

        if (c == '=') {
            add_keyword(EQUALITY);
        } else if (c == ';') {
            add_keyword(DELIMITER);
        } else if (c == '|') {
            const bool join_next = peek() == '|';

            add_keyword(join_next ? OR : PIPE);
            if (join_next) consume();
        } else if (c == '&') {
            const bool join_next = peek() == '&';

            if (!join_next) error("`&` is not a valid symbol, `&&` is. Peek is `%c`\n", peek());
            else {
                add_keyword(AND);
                consume();
            }
        } else if (c == '-') {
            const bool join_next = peek() == '>';

            if (!join_next) error("`-` is not a valid symbol, `->` is. Peek is `%c`\n", peek());
            else {
                add_keyword(ARROW);
                consume();
            }
        } else if (is_whitespace(c) || is_newline(c)) {
            // ignore whitespace & newlines
        } else {
            error("Invalid character `%c` provided, unable to lex to token\n", c);
        }

        consume();

    skip_consume:
        continue;
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

char peek() {
    if (!is_valid_index(c_pos + 1)) return '\0';

    return c_line->data[c_pos + 1];
}

char current() {
    if (!is_valid_index(c_pos)) return '\0';

    return c_line->data[c_pos];
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

    if (type == OPERATOR || type == TYPE) {
        printf("::%u", token->data.pos);
    }
    else if (type == IDENTIFIER) {
        printf("::%s", token->data.str);
    }
}