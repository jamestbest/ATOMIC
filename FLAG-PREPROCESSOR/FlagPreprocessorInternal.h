//
// Created by jamescoward on 14/09/2024.
//

#ifndef FLAGPREPROCESSORINTERNAL_H
#define FLAGPREPROCESSORINTERNAL_H

#include "../SharedIncludes/Vector.h"
#include "../SharedIncludes/Colours.h"
#include "../SharedIncludes/Flag_shared.h"

#include "../SharedIncludes/Helper_File.h"

#include <time.h>

#define Error(type, msg) C_RED type C_RST msg

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

#define STATIC_STRING_LEN(str)              sizeof (str) - 1

#define ATOM_FP__HEADER_GUARD               "ATOM_FLAGS_H"

#define ATOM_FP__TEMP_FILENAME_STARTER      "ATOM_TEMP_FLAG_DATA"

#define ATOM_FP__ENUM_FLAGS_NAME            "ATOM_CT__FLAGS"
#define ATOM_FP__ENUM_OPTIONS_NAME          "ATOM_CT__OPTIONS"

#define ATOM_FP__OPTION_START               "ATOM_CT__OPTION_"
#define ATOM_FP__FLAG_START                 "ATOM_CT__FLAG_"

#define ATOM_FP__FLAGS_COUNT_NAME           "COUNT"
#define ATOM_FP__OPTIONS_COUNT_NAME         "COUNT"

#define ATOM_FP__FLAG_INFO_STRUCT_NAME      "FlagInfo"
#define ATOM_FP__FLAG_INFO_INSTANCE_NAME    "flag_info"
#define ATOM_FP__FLAG_INFO_STRUCT_STR_NAME  "flag_name"
#define ATOM_FP__FLAG_INFO_STRUCT_DEF_NAME  "default_value"

#define ATOM_FP__KEYWORD_FLAG               "FLAG"
#define ATOM_FP__KEYWORD_OPTION             "OPTION"

#define ATOM_FP__FLAGS_ARRAY_NAME           "ATOM_VR__FLAGS"

#define ATOM_CT__FLAGS_PRE_END              "//%%END%%"

/* FLAGS.c.c */
#define ATOM_CT__FLAGS_PRE_OPT_ENUM         "//%%OPTION ENUM%%"
#define ATOM_CT__FLAGS_PRE_FLG_ENUM         "//%%FLAG ENUM%%"

#define ATOM_CT__FLAGS_PRE_FLG_IDX_SWT      "//%%FLAG INDEX SWITCH%%"
#define ATOM_CT__FLAGS_PRE_FLG_STR_SWT      "//%%FLAG STR SWITCH%%"

#define ATOM_CT__FLAGS_PRE_OPT_START        "ATOM_CT__OPTION_"
#define ATOM_CT__FLAGS_PRE_FLG_START        "ATOM_CT__FLAG_"

/* FLAGS.h */
#define ATOM_CT__FLAGS_PRE_OPT_DEF          "//%%OPTION DEFINE%%"
#define ATOM_CT__FLAGS_PRE_FLG_DEF          "//%%FLAG DEFINE%%"

#define ATOM_CT__FLAGS_PRE_FLG_STR          "//%%FLAG STRINGS%%"
#define ATOM_CT__FLAGS_PRE_OPT_STR          "//%%OPTION STRINGS%%"

#endif //FLAGPREPROCESSORINTERNAL_H
