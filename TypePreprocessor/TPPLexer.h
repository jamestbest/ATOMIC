//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXER_H
#define TPPLEXER_H

#include "SharedIncludes/Array.h"
#include "SharedIncludes/Buffer.h"
#include "output/shared_types.h"

typedef enum TPPType {
    KEYWORD,
    KEYVALUE,

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
typedef enum KEYWORDS {
    ALIASES,
    BI,
    COERCIONS,
    DEFAULT,
    LEFT,
    LRVALUES,
    OPERANDS,
    OPERATORS,
    OVER,
    POSTFIX,
    PREFIX,
    REQUIRE,
    RIGHT,
    SIZE,
    TRI,
    TYPEFIX,
    TYPES,
    UNARY,
    UNWRAP,
    VIRTUAL,
    WRAP,
    KEYWORD_COUNT
} KEYWORDS;

typedef struct TPPToken {
    TPPType type;
    union {
        char* str;
        unsigned int pos;
        long long numeric;
        KEYWORDS keyword;
        KEYVALUES keyvalue;
    } data;
} TPPToken;

ARRAY_PROTO(TPPToken, TPPToken)

extern const char* KEYVALUE_STRINGS[KV_COUNT];

uint tpplex_setup(const Vector* type_enum, const Vector* operator_enum);
void tpplex_line(const Buffer* line_buffer);
TPPTokenArray tpplex_end();

void print_tpptoken(const TPPToken* token);
void print_tpptoken_type(TPPType type);

const char* get_tpptoken_type_string(TPPType type);
const char* get_tpptoken_keyword_string(KEYWORDS keyword);
const char* get_tpptoken_keyvalue_string(KEYVALUES kv);

#endif //TPPLEXER_H
