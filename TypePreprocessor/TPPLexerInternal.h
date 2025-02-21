//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXERINTERNAL_H
#define TPPLEXERINTERNAL_H

#include "../SharedIncludes/Colours.h"
#include "TPPLexer.h"

#define COMP_STR_LEN(STR) (sizeof (STR) - 1)
#define DBG_SPECIAL_SEQ 0x12345678

const char
    KEYWORD_ALIASES[] = "ALIASES",
    KEYWORD_TYPES[] = "TYPES",
    KEYWORD_OPERATORS[] = "OPERATORS",
    KEYWORD_COERCIONS[] = "COERCIONS",
    KEYWORD_OPERANDS[] = "OPERANDS";

const char* TPPTypesStrings[TPPTYPE_COUNT] = {
    [ALIASES] = KEYWORD_ALIASES,
    [TYPES] = KEYWORD_TYPES,
    [OPERATORS] = KEYWORD_OPERATORS,
    [COERCIONS] = KEYWORD_COERCIONS,
    [OPERANDS] = KEYWORD_OPERANDS,

    [IDENTIFIER] = "IDENTIFIER",
    [NUMERIC] = "NUMERIC",

    [EQUALITY] = "EQUALITY (=)",
    [ARROW] = "ARROW (->)",
    [PIPE] = "PIPE (|)",
    [AND] = "AND (&&)",
    [OR] = "OR (||)",
    [DELIMITER] = "DELIMITER (;)",

    [EOS] = C_YLW"EOS"C_RST,
};

#endif //TPPLEXERINTERNAL_H
