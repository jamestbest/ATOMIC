//
// Created by jamescoward on 14/09/2024.
//

#ifndef FLAGPREPROCESSORINTERNAL_H
#define FLAGPREPROCESSORINTERNAL_H

#include "../SharedIncludes/Vector.h"
#include "../SharedIncludes/Colours.h"

#include "../SharedIncludes/Helper_File.h"

#include <time.h>

#define Error(type, msg) C_RED type C_RST msg

typedef enum Keywords {
    FP_KEYWORD_ARG,
    FP_KEYWORD_FLAG,
    FP_KEYWORD_FOR,
    FP_KEYWORD_FROM,
    FP_KEYWORD_OPTION,
    FP_KEYWORD_TYPE,
    FP_KEYWORD_COUNT,
} Keywords;

const char* const keyword_str[FP_KEYWORD_COUNT] = {
    [FP_KEYWORD_ARG] = "ARG",
    [FP_KEYWORD_FLAG] = "FLAG",
    [FP_KEYWORD_FOR] = "FOR",
    [FP_KEYWORD_FROM] = "FROM",
    [FP_KEYWORD_OPTION] = "OPTION",
    [FP_KEYWORD_TYPE] = "TYPE",
};
_Static_assert(sizeof(keyword_str) / sizeof(keyword_str[0]) == FP_KEYWORD_COUNT);

typedef enum Types {
    FP_TYPE_BOOL,
    FP_TYPE_CHARACTER,
    FP_TYPE_INTEGER,
    FP_TYPE_STR,
    FP_TYPES_COUNT,
} Types;

const char* const types_str[FP_TYPES_COUNT] = {
    [FP_TYPE_BOOL] = "BOOL",
    [FP_TYPE_CHARACTER] = "CHAR",
    [FP_TYPE_INTEGER] = "INT",
    [FP_TYPE_STR] = "STR",
};
_Static_assert(sizeof(types_str) / sizeof(types_str[0]) == FP_TYPES_COUNT);

typedef struct KeywordInfo {
    const char* name;
    Keywords enum_pos;
} KeywordInfo;

typedef enum TokenType {
    FP_KEYWORD,
    FP_IDENTIFIER,
    FP_LIT_BOOL,
    FP_LIT_INT,
    FP_INVALID,
    FP_COUNT,
} TokenType;

const char* const token_types_str[FP_COUNT] = {
    [FP_KEYWORD] = C_RED"KEYWORD"C_RST,
    [FP_IDENTIFIER] = C_BLU"IDENTIFIER"C_RST,
    [FP_LIT_BOOL] = C_GRN"BOOL"C_RST,
    [FP_LIT_INT] = C_GRN"INT"C_RST,
    [FP_INVALID] = C_RED"INVALID"C_RST,
};

typedef struct FPToken {
    TokenType type;
    union {
        const char* str;
        Keywords keyword;
        bool boolean;
        long long integer;
    };
} FPToken;

typedef struct FlagInfo {
    const char* flag_name;
    bool default_value;
} FlagInfo;

typedef enum State {
    SEARCHING,

    FOUND_OPT_ENUM,
    FOUND_FLG_ENUM,

    FOUND_FLG_IDX_SWT,
    FOUND_FLG_STR_SWT,

    FOUND_OPT_DEF,
    FOUND_FLG_DEF,

    FOUND_FLG_STR,
} State;


void cleanup_write(FILE* file, FILE* nfile, Buffer* buffer);
void cleanup_skip(FILE* file, FILE* nfile, Buffer* buffer);

void close_files(uint count, ...);

void collect_enums(FILE* file, FILE* nfile, const char* prefix,
                   Buffer* buffer, Vector* enum_vec);
void free_enums(Vector* enums);

void parse_c(FILE* cptr, FILE* ncptr, Vector* flag_enums);
void parse_h(FILE* hptr, FILE* nhptr, Vector* flag_enums, Vector* option_enums);

void parse_switch(FILE* file, FILE* nfile, const char* prefix,
                  const char* to, const char* from, const char* default_value,
                  Buffer* buffer, Vector* flag_enums);

void parse_def(FILE* file, FILE* nfile, Vector* enums, const char* prefix, Buffer* buffer);
void parse_string(FILE* file, FILE* nfile, Vector* enums, const char* prefix, Buffer* buffer);

uint write_out_flag_data(const char* output_filename);

#define ATOM_FP__COMMENT_START             "//"

#define STATIC_STRING_LEN(str)              sizeof (str) - 1

#define ATOM_FP__HEADER_GUARD               "ATOM_FLAGS_H"

#define ATOM_FP__TEMP_FILENAME_STARTER      "ATOM_TEMP_FLAG_DATA"

#define ATOM_FP__ENUM_FLAGS_NAME            "ATOM_CT__FLAGS"
#define ATOM_FP__ENUM_OPTIONS_NAME          "ATOM_CT__OPTIONS"

#define ATOM_FP__OPTION_START               "ATOM_CT__OPTION_"
#define ATOM_FP__FLAG_START                 "ATOM_CT__FLAG_"

#define ATOM_FP__FLAGS_ARRAY_NAME           "ATOM_VR__FLAGS"

#define ATOM_FP__FLAGS_COUNT_NAME           "COUNT"
#define ATOM_FP__OPTIONS_COUNT_NAME         "COUNT"

#define ATOM_FP__FLAG_INFO_STRUCT_NAME      "FlagInfo"
#define ATOM_FP__FLAG_INFO_INSTANCE_NAME    "flag_info"
#define ATOM_FP__FLAG_INFO_STRUCT_STR_NAME  "flag_name"
#define ATOM_FP__FLAG_INFO_STRUCT_DEF_NAME  "default_value"

#define ATOM_FP__MAX_STR_PRINT_PREVIEW      15

#endif //FLAGPREPROCESSORINTERNAL_H
