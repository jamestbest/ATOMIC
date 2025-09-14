//
// Created by jamescoward on 27/08/2025.
//

#ifndef ATOMIC_TPPHEADER_H
#define ATOMIC_TPPHEADER_H

#include <stdint.h>

typedef struct OpInfo {
    uint8_t precedence: 5;
    uint8_t assoc: 1;
    uint8_t type: 2;
} OpInfo;

typedef enum OutputType {
    OUT_BASED,
    OUT_EXPLICIT,
    OUT_LEFT,
    OUT_RIGHT,
    OUT_UNWRAP,
    OUT_COUNT
} OutputType;

typedef enum TYPE_LIKE_TYPES {
    TYPE_FIX,
    TYPE,
    TYPE_LIKE_TYPES_COUNT
} TYPE_LIKE_TYPES;

typedef struct OperandInfo {
    union {
        uint8_t* matrix;
        uint64_t typemap;
    };

    OutputType out_type         :  3;
    uint16_t output_index       : 11; // for explicit outputs
    uint16_t op_type            :  2;
} OperandInfo;

typedef struct uintArrayS {
    const size_t size;
    const unsigned int* arr;
} uintArrayS;

typedef struct TypeLikeInfo {
    uint8_t size;
    TYPE_LIKE_TYPES type;
} TypeLikeInfo;

typedef struct TypeInfo {
    TypeLikeInfo base;

    uintArrayS sizes;
    uintArrayS reqs;

    uint8_t has_variable_sizes: 1;
    uint8_t is_virtual        : 1;
} TypeInfo;

typedef struct TypefixInfo {
    TypeLikeInfo base;

    uint8_t prefix: 1;
} TypefixInfo;

typedef union TypeLike {
    TypefixInfo tf;
    TypeInfo t;
} TypeLike;

typedef uint8_t* TypeMatrix;

#endif // ATOMIC_TPPHEADER_H
