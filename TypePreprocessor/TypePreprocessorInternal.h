//
// Created by jamescoward on 20/07/2024.
//

#ifndef TYPEPREPROCESSORINTERNAL_H
#define TYPEPREPROCESSORINTERNAL_H

#include <stdbool.h>

#include "TPPLexer.h"
#include "SharedIncludes/Vector.h"
#include "TPPParser.h"

#include "TypePreprocessor.h"

ARRAY_PROTO(Vector, Vector)

typedef struct OpInfo {
    uint enum_position;
    TypeMatrix type_matrix;
} OpInfo;

typedef enum OperatorSeekState {
    SEEK_OPERATOR_NAME,
    SEEK_SYMMETRIC_CHECK,
    SEEK_TYPES,
} OperatorSeekState;

const char* OUTPUT_TYPE_STRINGS[OUT_COUNT + 1]= {
    [OUT_BASED]= "BASED",
    [OUT_EXPLICIT]= "EXPLICIT",
    [OUT_LEFT]= "LEFT",
    [OUT_RIGHT]= "RIGHT",
    [OUT_UNWRAP]= "UNWRAPPED",
    [OUT_COUNT]= "[[ERROR]] USAGE OF COUNT ENUM"
};

#endif //TYPEPREPROCESSORINTERNAL_H
