//
// Created by jamescoward on 19/09/2025.
//

#ifndef ATOMIC_SHARED_TYPES_H
#define ATOMIC_SHARED_TYPES_H

typedef enum OutputType {
    OUT_BASED,
    OUT_EXPLICIT,
    OUT_LEFT,
    OUT_RIGHT,
    OUT_UNWRAP,
    OUT_WRAP,
    OUT_COUNT
} OutputType;

extern const char* OUTPUT_TYPE_STRINGS[OUT_COUNT + 1];

typedef enum KEYVALUES {
    KV_ALL,
    KV_LVALUE,
    KV_RVALUE,
    KV_TYPE,
    KV_VARIABLE,
    KV_COUNT
} KEYVALUES;

typedef enum TYPE_LIKE_TYPES {
    TL_TYPE_FIX,
    TL_TYPE,
    TL_BUILTIN,
    TL_TYPE_LIKE_TYPES_COUNT
} TYPE_LIKE_TYPES;

typedef uint8_t* TypeMatrix;

typedef struct RuleValue {
    union {
        struct {
            uint16_t is_builtin: 1;
            uint16_t idx: 15;
        };
        uint16_t cmpable;
    };
} RuleValue;

typedef struct CoercionRule {
    RuleValue left;
    RuleValue right;
} CoercionRule;

typedef struct OperandInfoBase {
    union {
        TypeMatrix matrix; // for binary
        uint64_t typemap;  // for unary
        // [[todo]] how to support trinary operators?
    };

    RuleValue left;
    RuleValue right;

    OutputType out_type         :  3;
    uint16_t output_index       : 10; // for explicit outputs & wrapping
    uint16_t op_type            :  2;
    uint16_t is_keyvalue        :  1;
} OperandInfoBase;

#endif // ATOMIC_SHARED_TYPES_H
