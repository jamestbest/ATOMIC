//
// Created by jamescoward on 09/09/2025.
//

#ifndef ATOMIC_GENERATOR_H
#define ATOMIC_GENERATOR_H

#include "SharedIncludes/Array.h"
#include "Parser/Node.h"

#define MAX_REG   63
#define RET_REG   64
#define ARG_REG_S 128
#define ARG_REG_E 254
#define NO_REG    ((uint8_t)-1)

#define ARG_COUNT_MAX (ARG_REG_E - ARG_REG_S)

#define NO_FREED 255

typedef struct StringEntry {
    const char* string;
    size_t value;
} StringEntry;

ARRAY_PROTO(StringEntry, StringEntry)

typedef enum BYTE_CODE {
    B_LABEL,
    B_CALL,

    B_ARITH_BI,
    B_ARITH_U,

    B_B,
    B_BEQ,
    B_BNE,
    B_BGT,

    B_STORE,

    B_CAST,

    B_RET,

    B_KALL,

    B_EXIT,

    B_COUNT
} BYTE_CODE;

typedef struct VReg {
    uint id;
    bool in_use;

    union {
        uint64_t u64;
         int64_t i64;

        double_t d;

        void* ptr;
    } data;
} VReg;

typedef enum ValueType {
    VT_REG,

    VT_IMM_I,
    VT_IMM_F,
    VT_IMM_U,
    VT_LABEL
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        size_t label;
        uint reg;
        uint64_t u64;
         int64_t i64;
        long double f80;
    } data;
} Value;

ARRAY_PROTO(Value, Value)

typedef struct ByteStmt {
    BYTE_CODE code;

    uint arith_idx;

    ValueArray args; // function calls, arith ops, memory ops, coerce, ret, cocks, so ALL mostly
    Value arg;

    uint size; // most that return, Arith ops, memory ops, ret

    uint dest_reg; // arith ops, memory ops, coerce ops

    const char* func_name;
    size_t label_id;  // && for function calls
} ByteStmt;

ARRAY_PROTO(ByteStmt, ByteStmt)

typedef struct GeneratorRet {
    errcode err;
    ByteStmtArray arr;
    StringEntryArray function;
} GeneratorRet;

GeneratorRet generate_byte_code(Node* root, const char* entry_function, FILE* out);

#endif // ATOMIC_GENERATOR_H
