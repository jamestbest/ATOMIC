//
// Created by jamescoward on 27/08/2025.
//

#ifndef ATOMIC_TPPHEADER_H
#define ATOMIC_TPPHEADER_H

#include <stdint.h>
#include "shared_types.h"

typedef struct encodedType {
    uint64_t general: 4;    // 2^4= 16 different types
    uint64_t type: 5;       // 2^5= 32 different types
    uint64_t size: 4;       // 2^4= 16 bytes
    uint64_t tf_offset: 16; // typefix offset e.g. POINTER
    uint64_t is_builtin: 1;
    uint64_t builtin_idx: 3;
    uint64_t is_lvalue:  1;
    // todo to support multiple tfs need a map
} encodedType;

typedef struct OpInfo {
    uint8_t precedence: 5;
    uint8_t assoc: 1;
    uint8_t type: 2;
} OpInfo;

typedef OperandInfoBase OperandInfo;

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

extern OpInfo OP_INFO[42];

typedef enum DefaultOperatorInfoGType {
    OIGT_PREFIX,
    OIGT_POSTFIX,
    OIGT_UNARY,
    OIGT_BINARY,
    OIGT_TRINARY
} DefaultOperatorInfoGType;

#endif // ATOMIC_TPPHEADER_H
