//
// Created by jamescoward on 09/11/2023.
//

#ifndef ATOMIC_FLAG_PREPROCESSOR_H
#define ATOMIC_FLAG_PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../SharedIncludes/Vector.h"
#include "../SharedIncludes/Colours.h"
#include "../SharedIncludes/Flag_shared.h"

#include "../SharedIncludes/Helper_File.h"

#define Error(type, msg) C_RED type C_RST msg

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


void cleanup_write(FILE* file, FILE* nfile, char_vec* buffer);
void cleanup_skip(FILE* file, FILE* nfile, char_vec* buffer);

void collect_enums(FILE* file, FILE* nfile, const char* prefix,
                   char_vec* buffer, charp_vec* enum_vec);

void parse_c(FILE* cptr, FILE* ncptr, charp_vec* flag_enums, charp_vec* option_enums);
void parse_h(FILE* hptr, FILE* nhptr, charp_vec* flag_enums, charp_vec* option_enums);

void parse_switch(FILE* file, FILE* nfile, const char* prefix,
                  const char* to, const char* from, const char* default_value,
                  char_vec* buffer, charp_vec* flag_enums);

void parse_def(FILE* file, FILE* nfile, charp_vec* enums, const char* prefix, char_vec* buffer);
void parse_string(FILE* file, FILE* nfile, charp_vec* enums, const char* prefix, char_vec* buffer);

char* get_path(const char* dir, const char* file);

#define ATOM_CT__FLAGS_PRE_END              "//%%END%%"

/* FLAGS.c */
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

#endif //ATOMIC_FLAG_PREPROCESSOR_H
