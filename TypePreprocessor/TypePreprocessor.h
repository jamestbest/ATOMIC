//
// Created by jamescoward on 20/07/2024.
//

#ifndef TYPEPREPROCESSOR_H
#define TYPEPREPROCESSOR_H

#include <stdlib.h>

#include "SharedIncludes/Array.h"

ARRAY_PROTO(uint, uint)

typedef struct TypeFixInfo {
    const char* name;
    const char* symbol;

    unsigned char prefix: 1;
} TypeFixInfo;

typedef struct TypeInfo {
    const char* general_type;
    union {
        const char* prefix;
        const char* name;
    };

    bool has_variable_sizes;
    uintArray sizes; // Array of uint sizes

    bool is_virtual;

    //[[todo]] change to array of enum positions
    uintArray requirements; // Array of typefix positions
} TypeInfo;

typedef struct OperatorInfo {
    const char* name;
    const char* symbol;

    const uint precedence;

    unsigned char assoc: 1;
    unsigned char op_type: 2;
} OperatorInfo;

typedef struct AliasInfo {
    const char* name;
    uint64_t type_map;
} AliasInfo;

typedef struct TypeMatrix {
    uint64_t top_pad;
    uint64_t bottom_pad;
} TypeMatrix;

typedef struct CoercionInfo {
    TypeMatrix matrix;
} CoercionInfo;

typedef struct OperandInfo {
    size_t type;

    TypeMatrix matrix;
} OperandInfo;

typedef struct DefaultTypeFixInfo {
    unsigned char prefix: 1;         // Prefix, Postfix
} DefaultTypeFixInfo;

typedef struct DefaultTypesInfo {
} DefaultTypesInfo;

typedef enum DefaultOperatorInfoAssoc {
    OIA_LEFT,
    OIA_RIGHT
} DefaultOperatorInfoAssoc;

typedef enum DefaultOperatorInfoGType {
    OIGT_PREFIX,
    OIGT_POSTFIX,
    OIGT_BINARY,
    OIGT_TRINARY
} DefaultOperatorInfoGType;

typedef struct DefaultOperatorsInfo {
    unsigned char assoc: 1;          // Left, Right
    unsigned char general_type: 2;   // Prefix, Postfix, Binary, Trinary
} DefaultOperatorsInfo;

typedef struct DefaultAliasesInfo {
} DefaultAliasesInfo;

typedef struct DefaultCoercionsInfo {
} DefaultCoercionsInfo;

typedef struct DefaultOperandsInfo {
} DefaultOperandsInfo;

typedef union DefaultInfoUnion {
    DefaultTypeFixInfo type_fix;
    DefaultTypesInfo types;
    DefaultOperatorsInfo operators;
    DefaultAliasesInfo aliases;
    DefaultCoercionsInfo coercions;
    DefaultOperandsInfo operands;
} DefaultInfoUnion;

typedef struct DefaultInfo {
    bool has_info;
    DefaultInfoUnion info;
} DefaultInfo;

#endif //TYPEPREPROCESSOR_H
