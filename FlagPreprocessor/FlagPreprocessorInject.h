//
// Created by jamescoward on 09/10/2024.
//

#ifndef FLAGPREPROCESSORINJECT_H
#define FLAGPREPROCESSORINJECT_H

#include "../SharedIncludes/Array.h"
#include "../SharedIncludes/Vector.h"

typedef enum Types {
    FP_TYPE_CHARACTER,
    FP_TYPE_INTEGER,
    FP_TYPE_STR,
    FP_TYPE_NATURAL,
    FP_TYPES_COUNT,
} Types;

static const char* const ATOM_FP__TYPES_STR[FP_TYPES_COUNT] = {
    [FP_TYPE_CHARACTER] = "CHAR",
    [FP_TYPE_INTEGER] = "INT",
    [FP_TYPE_STR] = "STR",
    [FP_TYPE_NATURAL] = "UINT",
};
_Static_assert(sizeof(ATOM_FP__TYPES_STR) / sizeof(ATOM_FP__TYPES_STR[0]) == FP_TYPES_COUNT);

typedef struct FlagInfo {
    const char* flag_name;
    bool default_value;
} FlagInfo;

typedef struct OptionArgInfo {
    const char* arg_name;
    bool repeated;
    Types type;
    Array arg_options;
} OptionArgInfo;

typedef struct OptionInfo {
    const char* option_name;

    Vector arg_infos;
} OptionInfo;

typedef union OptionData {
    const char* str;
    long long integer;
    unsigned long long natural;
    char character;
    size_t enum_pos;
} OptionData;

typedef struct StaticOptionArgInfo {
    const char* arg_name;
    const bool repeated;
    const Types type;
    const size_t arg_option_count;
    const OptionData* data;
} StaticOptionArgInfo;

typedef struct StaticOptionInfo {
    const char* option_name;
    const size_t arg_info_count;
    const StaticOptionArgInfo* args;
} StaticOptionInfo;

#endif //FLAGPREPROCESSORINJECT_H
