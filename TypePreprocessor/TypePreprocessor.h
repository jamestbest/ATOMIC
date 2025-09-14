//
// Created by jamescoward on 20/07/2024.
//

#ifndef TYPEPREPROCESSOR_H
#define TYPEPREPROCESSOR_H

#include "SharedIncludes/Array.h"
#include "TPPParser.h"

#include <stdlib.h>

ARRAY_PROTO(uint, uint)

typedef enum TYPE_LIKE_TYPES {
    TL_TYPE_FIX,
    TL_TYPE,
    TL_BUILTIN,
    TL_TYPE_LIKE_TYPES_COUNT
} TYPE_LIKE_TYPES;

typedef struct TypeLikeInfo {
    const char* general_type;
    union {
        const char* prefix;
        const char* name;
    };

    uint8_t size;
    TYPE_LIKE_TYPES type;
} TypeLikeInfo;

typedef struct BuiltinInfo {
    TypeLikeInfo base;

    LRBuiltInTypes idx;
} BuiltinInfo;

typedef struct TypeFixInfo {
    TypeLikeInfo base;
    const char* symbol;

    unsigned char prefix: 1;
} TypeFixInfo;

typedef struct TypeInfo {
    TypeLikeInfo base;

    uintArray sizes; // Array of uint sizes
    uintArray requirements; // Array of typefix positions

    uint8_t has_variable_sizes: 1;
    uint8_t is_virtual: 1;
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

typedef struct CoercionRuleValue {
    union {
        struct {
            uint16_t is_keyvalue: 1;
            uint16_t idx: 15;
        };
        uint16_t cmpable;
    };
} CoercionRuleValue;

typedef struct CoercionRule {
    CoercionRuleValue left;
    CoercionRuleValue right;
} CoercionRule;

int int_cmp(uint32_t a, uint32_t b);

typedef enum OutputType {
    OUT_BASED,
    OUT_EXPLICIT,
    OUT_LEFT,
    OUT_RIGHT,
    OUT_UNWRAP,
    OUT_COUNT
} OutputType;

extern const char* OUTPUT_TYPE_STRINGS[OUT_COUNT + 1];

typedef struct OperandInfo {
    OperatorInfo* operator;

    union {
        TypeMatrix matrix; // for binary
        uint64_t typemap;  // for unary
        // [[todo]] how to support trinary operators?
    };

    OutputType out_type         :  3;
    uint16_t output_index       : 11; // for explicit outputs
    uint16_t op_type            :  2;
} OperandInfo;

typedef struct LRValueData {
    union {
        struct {
            uint32_t is_builtin: 1;
            uint32_t idx: 3;
        };
        uint32_t cmpable;
    };
} LRValueData;

VECTOR_PROTO(TypeFixInfo , TypeFixInfo)
VECTOR_PROTO(TypeLikeInfo, TypeLikeInfo)
ARRAY_PROTO (OperatorInfo, OperatorInfo)
ARRAY_PROTO (AliasInfo   , AliasInfo)
ARRAY_PROTO (OperandInfo , OperandInfo)
ARRAY_PROTO (LRValueData , LRValueData)
ARRAY_PROTO (CoercionRule, CoercionRule)

typedef struct LRInfo {
    uint8_t is_left: 1;

    LRValueDataArray rules;
} LRInfo;

extern TypeLikeInfoVector typelikes;
extern TypeFixInfoVector typefix_mirror;
extern OperatorInfoArray operators;
extern AliasInfoArray aliases;

extern TypeMatrix coercions;
extern CoercionRuleArray coercion_rules;
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
