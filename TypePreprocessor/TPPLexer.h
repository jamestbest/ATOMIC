//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXER_H
#define TPPLEXER_H

#include "../SharedIncludes/Array.h"
#include "../SharedIncludes/Buffer.h"

typedef enum TPPType {
    KEYWORD,

    IDENTIFIER,
    CUSTOM_OPERATOR,
    NUMERIC,

    EQUALITY,
    ARROW,
    BIRROW,
    PIPE,
    AND,
    OR,
    DELIMITER,

    EOS, // End of Statement
    TPPTYPE_COUNT,
} TPPType;

// SORTED!
enum KEYWORDS {
    ALIASES,
    BI,
    COERCIONS,
    DEFAULT,
    LEFT,
    OPERANDS,
    OPERATORS,
    OVER,
    POSTFIX,
    PREFIX,
    REQUIRE,
    RIGHT,
    TRI,
    TYPEFIX,
    TYPES,
    VIRTUAL,
    KEYWORD_COUNT
};

typedef struct TPPToken {
    TPPType type;
    union {
        char* str;
        unsigned int pos;
        long long numeric;
        enum KEYWORDS keyword;
    } data;
} TPPToken;

ARRAY_PROTO(TPPToken, TPPToken)

uint tpplex_setup(const Vector* type_enum, const Vector* operator_enum);
void tpplex_line(const Buffer* line_buffer);
Array tpplex_end();

void print_tpptoken(const TPPToken* token);
void print_tpptoken_type(TPPType type);

const char* get_tpptoken_type_string(TPPType type);
const char* get_tpptoken_keyword_string(enum KEYWORDS keyword);

#endif //TPPLEXER_H
