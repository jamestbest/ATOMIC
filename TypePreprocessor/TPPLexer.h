//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXER_H
#define TPPLEXER_H

#include "../SharedIncludes/Array.h"
#include "../SharedIncludes/Buffer.h"

typedef enum TPPType {
    ALIAS,

    IDENTIFIER,
    OPERATOR,
    TYPE,

    EQUALITY,
    PIPE,
    AND,
    OR,
    DELIMITER,

    EOS, // End of Statement
} TPPType;

typedef struct TPPToken {
    TPPType type;
    union {
        char* str;
        unsigned int pos;
    } data;
} TPPToken;

ARRAY_PROTO(TPPToken, TPPToken)

uint tpplex_setup(const Vector* type_enum, const Vector* operator_enum);
void tpplex_line(const Buffer* line_buffer);
Array tpplex_end();

void print_tpptoken(const TPPToken* token);

#endif //TPPLEXER_H
