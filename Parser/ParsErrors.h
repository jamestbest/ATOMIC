//
// Created by jamescoward on 24/01/2024.
//

#ifndef ATOMIC_PARSERRORS_H
#define ATOMIC_PARSERRORS_H

#include "../Errors.h"

typedef enum ParsErrors {
    PARSERR_BLOCK_MISSING_BRACE,
} ParsErrors;

typedef enum ParseWarns {
    PARSEWARN_,
} ParseWarns;

//ERRORS
#define ATOM_CT__PARSERR_BLOCK_MISSING_BRACE "Block did not parse ending with curly brace." \
            "This may be because there is an error in the brace or because there is a missing brace"

//WARNINGS

#endif //ATOMIC_PARSERRORS_H
