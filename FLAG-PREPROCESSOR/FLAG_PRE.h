//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAG_PRE_H
#define ATOMIC_FLAG_PRE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../SharedIncludes/helper.h"
#include "../SharedIncludes/FLAG_Shared.h"

#define ATOM_CT__FLAG_PRE_END              "//%%END%%"
#define ATOM_CT__FLAG_PRE_DEF_START        "//%%FLAG DEFINE%%"

#define ATOM_CT__FLAG_PRE_IDX_SWT_START    "//%%FLAG INDEX SWITCH%%"
#define ATOM_CT__FLAG_PRE_STR_SWT_START    "//%%FLAG STR SWITCH%%"

#define ATOM_CT__FLAG_PRE_ENUM_START       "//%%FLAG ENUM%%"
#define ATOM_CT__FLAG_PRE_STRING_START     "//%%FLAG STRINGS%%"
#define ATOM_CT__FLAG_PRE_OPT_ENUM_START   "//%%OPTION ENUM%%"
#define ATOM_CT__FLAG_PRE_OPT_DEF_START    "//%%OPTION DEFINE%%"
#define ATOM_CT__FLAG_PRE_FLAG_REG         "ATOM_CT__FLAG_"
#define ATOM_CT__FLAG_PRE_OPT_REG          "ATOM_CT__OPTION_"

typedef enum State {
    SEARCHING,
    FOUND_FLAG_ENUM,
    FOUND_OPTION_ENUM,
    FOUND_STRINGS,
    FOUND_F_HASH,
    FOUND_O_HASH,
    FOUND_IDX_SWITCH,
    FOUND_STR_SWITCH,
} State;

typedef struct Enums {
    char** enumNames;
    uint count;
} Enums;

#define BUFF_SIZE 100
#define ENUM_BUFF_MIN_SIZE 10

Enums readEnums(FILE* fptr, FILE* nfile, char buff[BUFF_SIZE], char* enum_prefix);
void parseStrings(FILE* nfile, Enums enums, char* prefix);
void parseDefs(FILE* nfile, Enums enums, char* prefix);
void parseIdxSwitch(FILE* nfile, Enums enums);
void parseStrSwitch(FILE* nfile, Enums enums);
void cleanup(FILE* fptr, FILE* nfile, char* buff);

#endif //ATOMIC_FLAG_PRE_H
