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
    const char* alt_name;

    unsigned char prefix: 1;
} TypeFixInfo;

typedef struct TypeInfo {
    const char* general_type;
    union {
        const char* prefix;
        const char* name;
    };

    union {
        uintArray sizes; // Array of uint sizes
        Vector names;
    };

    uintArray requirements; // Array of typefix positions

    bool has_variable_sizes: 1;
    bool has_multiple_names: 1;
    bool is_virtual: 1;
} TypeInfo;

typedef struct OperatorInfo {
    const char* name;
    const char* symbol;

    const uint precedence;

    uint8_t assoc: 1;
    uint8_t op_type: 4;
    uint8_t symbol_is_ident: 1;
} OperatorInfo;

typedef struct AliasInfo {
    const char* name;
    uint64_t type_map;
} AliasInfo;

typedef uint8_t* TypeMatrix;

typedef struct CoercionInfo {
    TypeMatrix matrix;
} CoercionInfo;

typedef struct OperandInfo {
    OperatorInfo* operator;

    union {
        TypeMatrix matrix; // for binary
        uint64_t typemap;  // for unary
        // [[todo]] how to support trinary operators?
    };

    uint16_t explicit_out_type  :  1;
    uint16_t unwrapped_output   :  1;
    uint16_t output_index       : 14;

    unsigned char op_type: 2;
} OperandInfo;

ARRAY_PROTO(TypeFixInfo , TypeFixInfo)
ARRAY_PROTO(TypeInfo    , TypeInfo)
ARRAY_PROTO(OperatorInfo, OperatorInfo)
ARRAY_PROTO(AliasInfo   , AliasInfo)
ARRAY_PROTO(OperandInfo , OperandInfo)

extern TypeFixInfoArray typefixes;
extern TypeInfoArray types;
extern OperatorInfoArray operators;
extern AliasInfoArray aliases;

extern TypeMatrix coercions;
extern OperandInfoArray operands;

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
    OIGT_UNARY,
    OIGT_BINARY,
    OIGT_TRINARY
} DefaultOperatorInfoGType;

typedef struct DefaultOperatorsInfo {
    unsigned char assoc: 1;          // Left, Right
    unsigned char general_type: 3;   // Unary, Prefix, Postfix, Binary, Trinary
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
