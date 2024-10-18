//
// Created by jamescoward on 20/07/2024.
//

#ifndef TYPEPREPROCESSORINTERNAL_H
#define TYPEPREPROCESSORINTERNAL_H

#include <stdbool.h>
#include "TPPLexer.h"
#include "../SharedIncludes/Vector.h"
#include "TPPParser.h"

ARRAY_PROTO(Vector, Vector)

typedef struct OperatorInfo {
    const char* operator;
    bool symmetrical;
    Array types; //Array of Vectors of types
} OperatorInfo;

typedef struct TypeMatrix {
    uint64_t top_pad;
    uint64_t bottom_pad;
} TypeMatrix;

typedef struct OpInfo {
    uint enum_position;
    TypeMatrix type_matrix;
} OpInfo;

typedef struct TypeAlias {
    const char* identifier;
    uint32_t types; // just a bit map of all the types with their enum position being their bit position
} TypeAlias;

ARRAY_PROTO(struct OperatorInfo, OperatorInfo)

typedef enum OperatorSeekState {
    SEEK_OPERATOR_NAME,
    SEEK_SYMMETRIC_CHECK,
    SEEK_TYPES,
} OperatorSeekState;

#endif //TYPEPREPROCESSORINTERNAL_H
