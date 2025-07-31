//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXERINTERNAL_H
#define TPPLEXERINTERNAL_H

#include "../SharedIncludes/Colours.h"
#include "TPPLexer.h"

#define COMP_STR_LEN(STR) (sizeof (STR) - 1)
#define DBG_SPECIAL_SEQ 0x12345678


const char* KEYWORD_STRINGS[KEYWORD_COUNT] = {
    [ALIASES] = "ALIASES",
    [TYPES] = "TYPES",
    [OPERATORS] = "OPERATORS",
    [COERCIONS] = "COERCIONS",
    [OPERANDS] = "OPERANDS",
    [DEFAULT] = "DEFAULT",
    [LEFT] = "LEFT",
    [RIGHT] = "RIGHT",
    [POSTFIX] = "POSTFIX",
    [PREFIX] = "PREFIX",
    [TYPEFIX] = "TYPEFIX",
    [VIRTUAL] = "VIRTUAL",
    [BI] = "BI",
    [TRI] = "TRI",
    [OVER] = "OVER",
    [REQUIRE] = "REQUIRE",
    [UNWRAP] = "UNWRAP",
    [UNARY] = "UNARY",
};

const char* TPPTypesStrings[TPPTYPE_COUNT] = {
    [KEYWORD] = "KEYWORD",
    [IDENTIFIER] = "IDENTIFIER",
    [CUSTOM_OPERATOR] = "CUSTOM OP",
    [NUMERIC] = "NUMERIC",

    [EQUALITY] = "EQUALITY (=)",
    [ARROW] = "ARROW (->)",
    [BIRROW] = "BIRROW (<->)",
    [PIPE] = "PIPE (|)",
    [AND] = "AND (&&)",
    [OR] = "OR (||)",
    [DELIMITER] = "DELIMITER (;)",

    [EOS] = C_YLW"EOS"C_RST,
};

#endif //TPPLEXERINTERNAL_H
