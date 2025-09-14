//
// Created by jamescoward on 10/06/2025.
//

#ifndef ATOMIC_TPPGENERATORINTERNAL_H
#define ATOMIC_TPPGENERATORINTERNAL_H

#define OP_ENUM_PREFIX      "OP_"
#define TYPE_ENUM_PREFIX    "TYPE_"
#define GENERAL_ENUM_PREFIX "GTYPE_"

#define TYPES_ARRAY_NAME        "ATOM_CT__LEX_TYPES_RAW"
#define GENERAL_ARRAY_NAME      "ATOM_CT__LEX_TYPES_GENERAL_RAW"
#define TYPES_SMALL_ARRAY_NAME  "ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW"

#define OP_ARRAY_NAME       "ATOM_CT__LEX_OPERATORS_RAW"

#define OP_INFO_ARRAY_NAME  "OP_INFO"
#define TYPE_INFO_ARRAY_NAME "TYPE_INFO"

#define COERCION_MATRIX_NAME "COERCION_INFO"

// credit https://stackoverflow.com/a/19885112
const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

#endif // ATOMIC_TPPGENERATORINTERNAL_H
