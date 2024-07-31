//
// Created by jamescoward on 20/07/2024.
//

#ifndef TYPEPREPROCESSORINTERNAL_H
#define TYPEPREPROCESSORINTERNAL_H

#include <stdbool.h>
#include "TPPLexer.h"
#include "../SharedIncludes/Vector.h"

ARRAY_PROTO(Vector, Vector)
ARRAY_ADD(Vector, Vector)

typedef struct OperatorInfo {
    const char* operator;
    bool symmetrical;
    Array types; //Array of Vectors of types
} OperatorInfo;

typedef struct OpInfo {
    const char* operator;
} OpInfo;

ARRAY_PROTO(struct OperatorInfo, OperatorInfo)
ARRAY_ADD(struct OperatorInfo, OperatorInfo)

typedef enum OperatorSeekState {
    SEEK_OPERATOR_NAME,
    SEEK_SYMMETRIC_CHECK,
    SEEK_TYPES,
} OperatorSeekState;

#endif //TYPEPREPROCESSORINTERNAL_H
