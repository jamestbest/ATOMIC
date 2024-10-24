//
// Created by jamescoward on 29/07/2024.
//

#ifndef TPPLEXERINTERNAL_H
#define TPPLEXERINTERNAL_H

#include "../SharedIncludes/Buffer.h"
#include "../SharedIncludes/Colours.h"
#include "TPPLexer.h"

#define COMP_STR_LEN(STR) (sizeof (STR) - 1)
#define DBG_SPECIAL_SEQ 0x12345678

const char KEYWORD_ALIAS[] = "ALIAS";

const char* TPPTypesStrings[TPPTYPE_COUNT] = {
    [ALIAS] = "ALIAS",

    [IDENTIFIER] = "IDENTIFIER",
    [OPERATOR] = "OPERATOR",
    [TYPE] = "TYPE",

    [EQUALITY] = "EQUALITY (=)",
    [ARROW] = "ARROW (->)",
    [PIPE] = "PIPE (|)",
    [AND] = "AND (&&)",
    [OR] = "OR (||)",
    [DELIMITER] = "DELIMITER (;)",

    [EOS] = C_YLW"EOS"C_RST,
};

// _Static_assert(TPPTypesStrings[DELIMITER][0] == 'D');

#endif //TPPLEXERINTERNAL_H
