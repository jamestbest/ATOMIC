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

typedef struct OperatorInfo_OLD {
    const char* operator;
    bool symmetrical;
    Array types; //Array of Vectors of types
} OperatorInfo_OLD;

typedef struct OpInfo {
    uint enum_position;
    TypeMatrix type_matrix;
} OpInfo;

typedef enum OperatorSeekState {
    SEEK_OPERATOR_NAME,
    SEEK_SYMMETRIC_CHECK,
    SEEK_TYPES,
} OperatorSeekState;

#endif //TYPEPREPROCESSORINTERNAL_H
